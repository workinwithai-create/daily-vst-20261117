#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class NovemberResonanceAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    NovemberResonanceAudioProcessorEditor(NovemberResonanceAudioProcessor&);
    ~NovemberResonanceAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    NovemberResonanceAudioProcessor& audioProcessor;

    juce::Slider cutoffSlider, resonanceSlider, attackSlider, decaySlider, sustainSlider, releaseSlider, atAmountSlider;
    juce::Label cutoffLabel, resonanceLabel, attackLabel, decayLabel, sustainLabel, releaseLabel, atAmountLabel;

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment> cutoffAttachment, resonanceAttachment, attackAttachment, decayAttachment, sustainAttachment, releaseAttachment, atAmountAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NovemberResonanceAudioProcessorEditor)
};
