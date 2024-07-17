#pragma once
#include "../PluginProcessor.h"
class AmatiSliderParameterAttachment final : private juce::Slider::Listener
{
public:
    AmatiSliderParameterAttachment (
        juce::RangedAudioParameter& parameter,
        juce::Slider& slider,
        int index,
        const std::function<double (int, double)>&,
        const std::function<double (int, double)>&,
        juce::UndoManager* undoManager = nullptr);

    ~AmatiSliderParameterAttachment() override;

    void sendInitialUpdate();

private:
    void setValue (float newValue);
    void sliderValueChanged (juce::Slider*) override;

    void sliderDragStarted (juce::Slider*) override { attachment.beginGesture(); }
    void sliderDragEnded (juce::Slider*) override { attachment.endGesture(); }
    std::function<double (int, double)> value2Ratio;
    std::function<double (int, double)> ratio2Value;

    juce::Slider& slider;
    juce::ParameterAttachment attachment;
    bool ignoreCallbacks = false;
    int index;
};

class AmatiSliderAttachment
{
public:
    AmatiSliderAttachment (
        double initalValue,
        const juce::AudioProcessorValueTreeState& stateToUse,
        const juce::String& parameterID,
        juce::Slider& slider,
        int index,
        const std::function<double (int, double)>&,
        const std::function<double (int, double)>&);

private:
    std::unique_ptr<AmatiSliderParameterAttachment> attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmatiSliderAttachment)
};

class ParamEditor final : public juce::Component
{
public:
    explicit ParamEditor (juce::AudioProcessorValueTreeState&);
    ~ParamEditor() noexcept override;

    void paint (juce::Graphics&) override {}
    void resized() override;

    void updateParameters (const std::vector<PluginProcessor::FaustParameter>&,
        const std::function<double (int, double)>&,
        const std::function<double (int, double)>&);

private:
    juce::AudioProcessorValueTreeState& valueTreeState;
    juce::OwnedArray<Component> components {};
    juce::OwnedArray<juce::Label> labels {};
    juce::OwnedArray<AmatiSliderAttachment> sliderAttachments {};
    juce::OwnedArray<juce::AudioProcessorValueTreeState::ButtonAttachment> buttonAttachments {};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParamEditor)
};