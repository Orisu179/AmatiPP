#pragma once
#include "../PluginProcessor.h"

class AmatiSliderParameterAttachment final : private juce::Slider::Listener
{
public:
    AmatiSliderParameterAttachment (
        int index,
        juce::RangedAudioParameter& param,
        juce::Slider& s,
        const std::function<double(int, double)>& value2Ratio,
        const std::function<double(int, double)>& ratio2Value,
        juce::UndoManager* undoManager = nullptr
        );

//    explicit AmatiSliderParameterAttachment (
//        FaustProgram::Parameter faustParam,
//        juce::RangedAudioParameter& param,
//        juce::Slider& s,
//        juce::UndoManager* undoManager = nullptr
//    );

    ~AmatiSliderParameterAttachment() override;

    void sendInitialUpdate();

private:
    void setValue (float newValue);
    void sliderValueChanged (juce::Slider*) override;

    void sliderDragStarted (juce::Slider*) override { attachment.beginGesture(); }
    void sliderDragEnded (juce::Slider*) override { attachment.endGesture(); }

    int idx;
    juce::Slider& slider;
    juce::ParameterAttachment attachment;
    bool ignoreCallbacks = false;
    const std::shared_ptr<ValueConverter> converter;
    std::function<double (int, double)> valueToRatio;
    std::function<double (int, double)> ratioToValue;
};

/**
 *  This class contains a ParameterAttachment that used for generating the Slider UI For the parameter
 *
 */
class AmatiSliderAttachment
{
public:
    AmatiSliderAttachment (
        const int index,
        const juce::AudioProcessorValueTreeState& valueTreeState,
        const juce::String& parameterID,
        juce::Slider& attachedSlider,
        const std::function<double (int, double)>& value2Ratio,
        const std::function<double (int, double)>& ratio2Value
        );

private:
    std::unique_ptr<AmatiSliderParameterAttachment> attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmatiSliderAttachment)
};