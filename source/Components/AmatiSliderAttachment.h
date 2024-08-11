#pragma once
#include "../PluginProcessor.h"

class AmatiSliderParameterAttachment final : private juce::Slider::Listener
{
public:
    AmatiSliderParameterAttachment (
        FaustProgram::Parameter& faustParam,
        juce::RangedAudioParameter& param,
        juce::Slider& s,
        juce::UndoManager* undoManager = nullptr
    );


    ~AmatiSliderParameterAttachment() override;

    void sendInitialUpdate();
    void setValue (float newValue);

private:
    void sliderValueChanged (juce::Slider*) override;

    void sliderDragStarted (juce::Slider*) override { attachment.beginGesture(); }
    void sliderDragEnded (juce::Slider*) override { attachment.endGesture(); }

    FaustProgram::Parameter curParam;
    juce::Slider& slider;
    juce::ParameterAttachment attachment;
    bool ignoreCallbacks = false;
};

/**
 *  This class contains a ParameterAttachment that used for generating the Slider UI For the parameter
 *
 */
class AmatiSliderAttachment
{
public:
    AmatiSliderAttachment (
        FaustProgram::Parameter& curParam,
        juce::AudioProcessorValueTreeState& valueTreeState,
        const juce::String& parameterID,
        juce::Slider& attachedSlider
        );

private:
    std::unique_ptr<AmatiSliderParameterAttachment> attachment;
    const juce::String id;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmatiSliderAttachment)
};