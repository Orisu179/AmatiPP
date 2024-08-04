#include "AmatiSliderAttachment.h"
#include <utility>

AmatiSliderParameterAttachment::AmatiSliderParameterAttachment (
    const int index,
    juce::RangedAudioParameter& param,
    juce::Slider& s,
    const std::function<double (int, double)>& value2Ratio,
    const std::function<double (int, double)>& ratio2Value,
    juce::UndoManager* undoManager): idx(index), slider (s),
                                      attachment(param, [this](const float f) { setValue (f);}, undoManager),
                                      valueToRatio(value2Ratio), ratioToValue(ratio2Value)

{
   sendInitialUpdate();
   slider.valueChanged();
   slider.addListener(this);
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
    const double convertedValue = ratioToValue (idx, newValue);
//    const double convertedValue = converter->faust2ui(newValue);
    const juce::ScopedValueSetter<bool> svs (ignoreCallbacks, true);
    slider.setValue (convertedValue, juce::sendNotificationSync);
}

void AmatiSliderParameterAttachment::sliderValueChanged (juce::Slider*)
{
    // convert from original to 0 to 1
    if (!ignoreCallbacks)
    {
        const auto value = slider.getValue();
//        const auto convertedValue = converter->ui2faust(value);
        const auto convertedValue = valueToRatio(idx, value);
        attachment.setValueAsPartOfGesture (static_cast<float> (convertedValue));
    }
}

AmatiSliderAttachment::AmatiSliderAttachment (
    const int index,
    const juce::AudioProcessorValueTreeState& valueTreeState,
    const juce::String& parameterID,
    juce::Slider& attachedSlider,
    const std::function<double (int, double)>& value2Ratio,
    const std::function<double (int, double)>& ratio2Value) {

    if(juce::RangedAudioParameter* parameter = valueTreeState.getParameter(parameterID); parameter) {
        if(value2Ratio) {
            const auto initValue = static_cast<float>(value2Ratio(index, attachedSlider.getValue()));
            parameter->setValueNotifyingHost(initValue);
            attachment = std::make_unique<AmatiSliderParameterAttachment>(
                index,
                *parameter,
                attachedSlider,
                value2Ratio,
                ratio2Value,
                valueTreeState.undoManager);
        }
    }
}