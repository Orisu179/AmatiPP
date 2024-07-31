#pragma once
#include "../PluginProcessor.h"

class AmatiSliderParameterAttachment final : private juce::Slider::Listener
{
public:
    AmatiSliderParameterAttachment (
        juce::RangedAudioParameter& param,
        juce::Slider& s,
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
    const std::shared_ptr<ValueConverter> converter;
};

/**
 *  This class contains a ParameterAttachment that used for generating the Slider UI For the parameter
 *
 */
class AmatiSliderAttachment
{
public:
    AmatiSliderAttachment (
        const juce::AudioProcessorValueTreeState& value_tree_state,
        const juce::String& parameterID,
        juce::Slider& attachedSlider,
        const std::shared_ptr<ValueConverter>& fConversion);

private:
    std::unique_ptr<AmatiSliderParameterAttachment> attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmatiSliderAttachment)
};