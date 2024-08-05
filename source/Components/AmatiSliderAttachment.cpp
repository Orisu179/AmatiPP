#include "AmatiSliderAttachment.h"
#include <utility>

AmatiSliderParameterAttachment::AmatiSliderParameterAttachment (FaustProgram::Parameter& faustParam, juce::RangedAudioParameter& param, juce::Slider& s, juce::UndoManager* undoManager) : curParam (std::move(faustParam)), slider (s),attachment(param, [this](const float f) { setValue (f);}, undoManager)
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
    const double convertedValue = curParam.ratio2Value(curParam.index, newValue);
    const juce::ScopedValueSetter<bool> svs (ignoreCallbacks, true);
    slider.setValue (convertedValue, juce::sendNotificationSync);
}

void AmatiSliderParameterAttachment::sliderValueChanged (juce::Slider*)
{
    // convert from original to 0 to 1
    if (!ignoreCallbacks)
    {
        const auto value = slider.getValue();
        const double convertedValue = curParam.value2Ratio(curParam.index, value);
        attachment.setValueAsPartOfGesture (static_cast<float> (convertedValue));
    }
}

AmatiSliderAttachment::AmatiSliderAttachment (FaustProgram::Parameter& curParam, const juce::AudioProcessorValueTreeState& valueTreeState, const juce::String& parameterID, juce::Slider& attachedSlider)
{

    if(juce::RangedAudioParameter* parameter = valueTreeState.getParameter(parameterID); parameter)
    {
        const auto initValue = static_cast<float>(curParam.value2Ratio(curParam.index, attachedSlider.getValue()));
        parameter->setValueNotifyingHost(initValue);
        attachment = std::make_unique<AmatiSliderParameterAttachment>(
            curParam,
            *parameter,
            attachedSlider,
            valueTreeState.undoManager
            );
    }

}