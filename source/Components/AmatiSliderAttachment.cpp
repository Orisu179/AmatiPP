#include "AmatiSliderAttachment.h"
#include <utility>

AmatiSliderParameterAttachment::AmatiSliderParameterAttachment (
    juce::RangedAudioParameter& param,
    juce::Slider& s,
    std::shared_ptr<ValueConverter> fConverter,
    juce::UndoManager* undoManager) : slider (s),
attachment (param, [this] (const float f) { setValue (f); }, undoManager),
                                      converter (std::move (fConverter))
{
    sendInitialUpdate();
    slider.valueChanged();
    slider.addListener (this);
}

AmatiSliderParameterAttachment::~AmatiSliderParameterAttachment()
{
    slider.removeListener (this);
}

void AmatiSliderParameterAttachment::sendInitialUpdate()
{
    attachment.sendInitialUpdate();
}

void AmatiSliderParameterAttachment::setValue (const float newValue)
{
    // sets the value from 0 to 1 back to the original value
        // const double convertedValue = ratioToValue (index, newValue);
    const double convertedValue = converter->faust2ui(newValue);
    const juce::ScopedValueSetter<bool> svs (ignoreCallbacks, true);
    slider.setValue (convertedValue, juce::sendNotificationSync);
}

void AmatiSliderParameterAttachment::sliderValueChanged (juce::Slider*)
{
    // convert from original to 0 to 1
    if (!ignoreCallbacks)
    {
        const auto value = slider.getValue();
        const auto convertedValue = converter->ui2faust(value);
        attachment.setValueAsPartOfGesture (static_cast<float> (convertedValue));
    }
}

AmatiSliderAttachment::AmatiSliderAttachment (
    const juce::AudioProcessorValueTreeState& value_tree_state,
    const juce::String& parameterID,
    juce::Slider& attachedSlider,
    const std::shared_ptr<ValueConverter>& fConversion)
{
    if (juce::RangedAudioParameter* parameter = value_tree_state.getParameter (parameterID); parameter)
    {
        if(fConversion)
        {
            const auto initValue = static_cast<float> (fConversion->ui2faust (attachedSlider.getValue()));
            parameter->setValueNotifyingHost (initValue);
            attachment = std::make_unique<AmatiSliderParameterAttachment> (
               *parameter,
               attachedSlider,
               fConversion,
               value_tree_state.undoManager
               );
        }
    }
}