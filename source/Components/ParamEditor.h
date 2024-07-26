#pragma once
#include "../PluginProcessor.h"
#include "SliderBuilder.h"
class AmatiSliderParameterAttachment final : private juce::Slider::Listener
{
public:
    AmatiSliderParameterAttachment (
        int idx,
        juce::RangedAudioParameter& param,
        juce::Slider& s,
        const std::function<double (int, double)>&,
        const std::function<double (int, double)>&,
        juce::UndoManager* undoManager = nullptr
        );

    ~AmatiSliderParameterAttachment() override;

    void sendInitialUpdate();

private:
    void setValue (float newValue);
    void sliderValueChanged (juce::Slider*) override;

    void sliderDragStarted (juce::Slider*) override { attachment.beginGesture(); }
    void sliderDragEnded (juce::Slider*) override { attachment.endGesture(); }

    juce::Slider& slider;
    juce::ParameterAttachment attachment;
    bool ignoreCallbacks = false;
    const std::function<double (int, double)>& valueToRatio;
    const std::function<double (int, double)>& ratioToValue;
    const int index;
};

class AmatiSliderAttachment
{
public:
    AmatiSliderAttachment (
        int index,
        const juce::AudioProcessorValueTreeState& stateToUse,
        const juce::String& parameterID,
        juce::Slider& slider,
        const std::function<double (int, double)>&,
        const std::function<double (int, double)>&);
    AmatiSliderAttachment (const juce::AudioProcessorValueTreeState& value_tree_state, const juce::String& id, const juce::Slider& slider, std::function<double(double)>, std::function<double(double)>);

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
    SliderBuilder builder;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParamEditor)
};