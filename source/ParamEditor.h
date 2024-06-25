#pragma once
#include "PluginProcessor.h"
class AmatiSliderParameterAttachment : private juce::Slider::Listener
{
public:
    AmatiSliderParameterAttachment (juce::RangedAudioParameter& parameter, juce::Slider& slider,
        juce::UndoManager* undoManager = nullptr);

    ~AmatiSliderParameterAttachment() override;

    void sendInitialUpdate();

private:
    void setValue (float newValue);
    void sliderValueChanged (juce::Slider*) override;

    void sliderDragStarted (juce::Slider*) override { attachment.beginGesture(); }
    void sliderDragEnded   (juce::Slider*) override { attachment.endGesture(); }

    juce::Slider& slider;
    juce::ParameterAttachment attachment;
    bool ignoreCallbacks = false;
};

class AmatiSliderAttachment
{
public:
    AmatiSliderAttachment (juce::AudioProcessorValueTreeState& stateToUse,
        const juce::String& parameterID,
        juce::Slider& slider
    );

private:
    std::unique_ptr<AmatiSliderParameterAttachment> attachment;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmatiSliderAttachment)
};


class ParamEditor : public juce::Component
{
public:
    ParamEditor (juce::AudioProcessorValueTreeState&);
    ~ParamEditor () noexcept override;

    void paint (juce::Graphics&) override {}
    void resized () override;

    void updateParameters(const std::vector<PluginProcessor::FaustParameter>&);

private:
    juce::AudioProcessorValueTreeState& valueTreeState;
    juce::OwnedArray<juce::Component> components{};
    juce::OwnedArray<juce::Label> labels{};
    juce::OwnedArray<AmatiSliderAttachment> sliderAttachments{};
    juce::OwnedArray<juce::AudioProcessorValueTreeState::ButtonAttachment> buttonAttachments{};

    static void createParameter(const PluginProcessor::FaustParameter&, juce::AudioProcessorValueTreeState &stateToUse);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParamEditor)
};

