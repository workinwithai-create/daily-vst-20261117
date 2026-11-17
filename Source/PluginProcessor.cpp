#include "PluginProcessor.h"
#include "PluginEditor.h"

ResonanceVoice::ResonanceVoice()
{
    osc.initialise([](float x) { return std::sin(x); }); // Pure sine for cinematic pad character
    filter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
}

void ResonanceVoice::prepare(const juce::dsp::ProcessSpec& spec)
{
    osc.prepare(spec);
    filter.prepare(spec);
    adsr.setSampleRate(spec.sampleRate);
    cutoffSmoothed.reset(spec.sampleRate, 0.05);
    resonanceSmoothed.reset(spec.sampleRate, 0.05);
}

void ResonanceVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int)
{
    currentMidiNote = midiNoteNumber;
    currentVelocity = velocity;
    auto freq = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    osc.setFrequency(freq);
    adsr.noteOn();
}

void ResonanceVoice::stopNote(float, bool allowTailOff)
{
    if (allowTailOff)
        adsr.noteOff();
    else
        adsr.reset();
}

void ResonanceVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (processor == nullptr || !adsr.isActive())
        return;

    auto& at = processor->getAftertouch();
    float aftertouch = at.load();

    auto* cutoffParam = processor->getAPVTS().getRawParameterValue("cutoff");
    auto* resParam = processor->getAPVTS().getRawParameterValue("resonance");
    auto* atAmountParam = processor->getAPVTS().getRawParameterValue("atAmount");
    auto* attackParam = processor->getAPVTS().getRawParameterValue("attack");
    auto* decayParam = processor->getAPVTS().getRawParameterValue("decay");
    auto* sustainParam = processor->getAPVTS().getRawParameterValue("sustain");
    auto* releaseParam = processor->getAPVTS().getRawParameterValue("release");

    juce::ADSR::Parameters adsrParams;
    adsrParams.attack = *attackParam;
    adsrParams.decay = *decayParam;
    adsrParams.sustain = *sustainParam;
    adsrParams.release = *releaseParam;
    adsr.setParameters(adsrParams);

    cutoffSmoothed.setTargetValue(*cutoffParam + aftertouch * *atAmountParam * 4000.0f);
    resonanceSmoothed.setTargetValue(*resParam);

    for (int i = 0; i < numSamples; ++i)
    {
        if (!adsr.isActive())
        {
            clearCurrentNote();
            break;
        }

        float env = adsr.getNextSample();
        float cutoff = cutoffSmoothed.getNextValue();
        float res = resonanceSmoothed.getNextValue();

        filter.setCutoffFrequency(juce::jlimit(20.0f, 20000.0f, cutoff));
        filter.setResonance(juce::jlimit(0.1f, 10.0f, res));

        float sample = osc.processSample(0.0f) * env * currentVelocity * 0.25f;
        sample = filter.processSample(0, sample);

        for (int ch = 0; ch < outputBuffer.getNumChannels(); ++ch)
            outputBuffer.addSample(ch, startSample + i, sample);
    }
}

//==============================================================================
NovemberResonanceAudioProcessor::NovemberResonanceAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "PARAMETERS", createParameterLayout())
{
    synth.addSound(new juce::SynthesiserSound());

    for (int i = 0; i < 8; ++i)
    {
        auto* voice = new ResonanceVoice();
        voice->setProcessor(this);
        synth.addVoice(voice);
    }
}

NovemberResonanceAudioProcessor::~NovemberResonanceAudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout NovemberResonanceAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>("cutoff", "Cutoff", juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.5f), 800.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("resonance", "Resonance", juce::NormalisableRange<float>(0.1f, 8.0f, 0.01f), 0.7f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("attack", "Attack", juce::NormalisableRange<float>(0.001f, 2.0f, 0.001f, 0.5f), 0.3f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("decay", "Decay", juce::NormalisableRange<float>(0.01f, 2.0f, 0.001f, 0.5f), 0.4f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("sustain", "Sustain", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.7f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("release", "Release", juce::NormalisableRange<float>(0.01f, 5.0f, 0.001f, 0.5f), 1.2f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("atAmount", "Aftertouch Amount", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));

    return { params.begin(), params.end() };
}

void NovemberResonanceAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = 1; // mono per voice processing

    limiter.prepare({sampleRate, static_cast<juce::uint32>(samplesPerBlock), 2});
    limiter.setThreshold(-1.0f);
    limiter.setRelease(100.0f);

    for (int i = 0; i < synth.getNumVoices(); ++i)
        if (auto* voice = dynamic_cast<ResonanceVoice*>(synth.getVoice(i)))
            voice->prepare(spec);
}

void NovemberResonanceAudioProcessor::releaseResources() {}

bool NovemberResonanceAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void NovemberResonanceAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    // Update aftertouch from channel pressure
    for (const auto metadata : midiMessages)
    {
        auto msg = metadata.getMessage();
        if (msg.isChannelPressure())
            aftertouchValue = msg.getChannelPressureValue() / 127.0f;
    }

    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    limiter.process(context);
}

juce::AudioProcessorEditor* NovemberResonanceAudioProcessor::createEditor()
{
    return new NovemberResonanceAudioProcessorEditor(*this);
}

void NovemberResonanceAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void NovemberResonanceAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NovemberResonanceAudioProcessor();
}
