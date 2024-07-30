#pragma once
#include "../PluginProcessor.h"

class AmatiSliderParameterAttachment final : private juce::Slider::Listener
{
public:
    // AmatiSliderParameterAttachment (
    //     int idx,
    //     juce::RangedAudioParameter& param,
    //     juce::Slider& s,
    //     const std::function<double (int, double)>&,
    //     const std::function<double (int, double)>&,
    //     juce::UndoManager* undoManager = nullptr
    //     );

    AmatiSliderParameterAttachment (
        juce::RangedAudioParameter& param,
        juce::Slider& slider,
        std::shared_ptr<ValueConverter> fConverter,
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
    // const std::function<double (int, double)>& valueToRatio;
    // const std::function<double (int, double)>& ratioToValue;
    const std::shared_ptr<ValueConverter> converter;
};

/**
 *  This class contains a ParameterAttachment that used for generating the Slider UI For the parameter
 *
 */
class AmatiSliderAttachment
{
public:
    // AmatiSliderAttachment (
    //     int index,
    //     const juce::AudioProcessorValueTreeState& stateToUse,
    //     const juce::String& parameterID,
    //     const juce::Slider& slider,
    //     const std::function<double (int, double)>&,
    //     const std::function<double (int, double)>&);
    AmatiSliderAttachment (
        const juce::AudioProcessorValueTreeState& value_tree_state,
        const juce::String& parameterID,
        juce::Slider& attachedSlider,
        const std::shared_ptr<ValueConverter>& fConversion);

private:
    std::unique_ptr<AmatiSliderParameterAttachment> attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmatiSliderAttachment)
};