#include "PluginEditor.h"

NovemberResonanceAudioProcessorEditor::NovemberResonanceAudioProcessorEditor(NovemberResonanceAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Dark cinematic UI setup
    setSize(700, 450);

    auto setupSlider = [this](juce::Slider& slider, juce::Label& label, const juce::String& text)
    {
        addAndMakeVisible(slider);
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
        slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xFF6A5ACD)); // slate blue
        slider.setColour(juce::Slider::thumbColourId, juce::Colour(0xFFE0E0FF));

        addAndMakeVisible(label);
        label.setText(text, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        label.setColour(juce::Label::textColourId, juce::Colours::white);
        label.attachToComponent(&slider, false);
    };

    setupSlider(cutoffSlider, cutoffLabel, "Cutoff");
    setupSlider(resonanceSlider, resonanceLabel, "Resonance");
    setupSlider(attackSlider, attackLabel, "Attack");
    setupSlider(decaySlider, decayLabel, "Decay");
    setupSlider(sustainSlider, sustainLabel, "Sustain");
    setupSlider(releaseSlider, releaseLabel, "Release");
    setupSlider(atAmountSlider, atAmountLabel, "Aftertouch");

    cutoffAttachment = std::make_unique<Attachment>(audioProcessor.getAPVTS(), "cutoff", cutoffSlider);
    resonanceAttachment = std::make_unique<Attachment>(audioProcessor.getAPVTS(), "resonance", resonanceSlider);
    attackAttachment = std::make_unique<Attachment>(audioProcessor.getAPVTS(), "attack", attackSlider);
    decayAttachment = std::make_unique<Attachment>(audioProcessor.getAPVTS(), "decay", decaySlider);
    sustainAttachment = std::make_unique<Attachment>(audioProcessor.getAPVTS(), "sustain", sustainSlider);
    releaseAttachment = std::make_unique<Attachment>(audioProcessor.getAPVTS(), "release", releaseSlider);
    atAmountAttachment = std::make_unique<Attachment>(audioProcessor.getAPVTS(), "atAmount", atAmountSlider);
}

NovemberResonanceAudioProcessorEditor::~NovemberResonanceAudioProcessorEditor() {}

void NovemberResonanceAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Dark cinematic background
    g.fillAll(juce::Colour(0xFF0D0D1A)); // deep navy/black

    g.setColour(juce::Colour(0xFF2A2A4A));
    g.fillRoundedRectangle(getLocalBounds().reduced(10).toFloat(), 12.0f);

    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(24.0f, juce::Font::bold));
    g.drawText("November Resonance", getLocalBounds().removeFromTop(50), juce::Justification::centred);

    g.setFont(juce::FontOptions(14.0f));
    g.setColour(juce::Colour(0xFFAAAAFF));
    g.drawText("Cinematic Pad  •  Aftertouch + Sustain Pedal", getLocalBounds().removeFromBottom(30), juce::Justification::centred);
}

void NovemberResonanceAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(20).removeFromTop(getHeight() - 80);
    auto row1 = area.removeFromTop(180);
    auto row2 = area;

    int sliderWidth = 90;
    cutoffSlider.setBounds(row1.removeFromLeft(sliderWidth));
    resonanceSlider.setBounds(row1.removeFromLeft(sliderWidth));
    atAmountSlider.setBounds(row1.removeFromLeft(sliderWidth));

    attackSlider.setBounds(row2.removeFromLeft(sliderWidth));
    decaySlider.setBounds(row2.removeFromLeft(sliderWidth));
    sustainSlider.setBounds(row2.removeFromLeft(sliderWidth));
    releaseSlider.setBounds(row2.removeFromLeft(sliderWidth));
}
