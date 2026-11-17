#pragma once

#include <JuceHeader.h>

class NovemberResonanceAudioProcessor;

class ResonanceVoice : public juce::SynthesiserVoice
{
public:
    ResonanceVoice();
    bool canPlaySound(juce::SynthesiserSound*) override { return true; }
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int newPitchWheelValue) override {}
    void controllerMoved(int controllerNumber, int newControllerValue) override {}
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    void setProcessor(NovemberResonanceAudioProcessor* p) { processor = p; }
    void prepare(const juce::dsp::ProcessSpec& spec);

private:
    NovemberResonanceAudioProcessor* processor = nullptr;
    juce::dsp::Oscillator<float> osc;
    juce::dsp::StateVariableTPTFilter<float> filter;
    juce::ADSR adsr;
    juce::SmoothedValue<float> cutoffSmoothed, resonanceSmoothed;
    float currentVelocity = 0.0f;
    int currentMidiNote = 60;
};

class NovemberResonanceAudioProcessor : public juce::AudioProcessor
{
public:
    NovemberResonanceAudioProcessor();
    ~NovemberResonanceAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int index) override {}
    const juce::String getProgramName(int index) override { return {}; }
    void changeProgramName(int index, const juce::String& newName) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }
    std::atomic<float>& getAftertouch() { return aftertouchValue; }

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    juce::Synthesiser synth;
    juce::AudioProcessorValueTreeState apvts;
    std::atomic<float> aftertouchValue { 0.0f };
    juce::dsp::Limiter<float> limiter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NovemberResonanceAudioProcessor)
};
