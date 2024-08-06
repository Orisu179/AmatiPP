#pragma once
#include "../PluginProcessor.h"
#include "AmatiSliderAttachment.h"
#include "SliderBuilder.h"
class ParamEditor final : public juce::Component
{
public:
    explicit ParamEditor (juce::AudioProcessorValueTreeState&);
    ~ParamEditor() noexcept override;

    void paint (juce::Graphics&) override {}
    void resized() override;

    void updateParameters (const std::vector<PluginProcessor::FaustParameter>&);
    void updateParameters (const std::vector<juce::Component>&);

private:
    juce::AudioProcessorValueTreeState& valueTreeState;
    juce::OwnedArray<Component> components {};
    juce::OwnedArray<juce::Label> labels {};
    juce::OwnedArray<AmatiSliderAttachment> sliderAttachments {};
    juce::OwnedArray<juce::AudioProcessorValueTreeState::ButtonAttachment> buttonAttachments {};
    SliderBuilder builder;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParamEditor)
};