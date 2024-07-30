#include "AmatiSliderAttachment.h"
AmatiSliderParameterAttachment::AmatiSliderParameterAttachment (
    const int idx,
    juce::RangedAudioParameter& param,
    juce::Slider& s,
    const std::function<double (int, double)>& value2Ratio,
    const std::function<double (int, double)>& ratio2Value,
    juce::UndoManager* undoManager) : slider (s),
                                      attachment (param, [this] (const float f) { setValue (f); }, undoManager),
                                      valueToRatio (value2Ratio),
                                      ratioToValue (ratio2Value),
                                      index (idx)
{
    sendInitialUpdate();
    slider.valueChanged();
    slider.addListener (this);
}

AmatiSliderParameterAttachment::~AmatiSliderParameterAttachment()
{
    slider.removeListener (this);
}

void AmatiSliderParameterAttachment::sendInitialUpdate() { attachment.sendInitialUpdate(); }

void AmatiSliderParameterAttachment::setValue (const float newValue)
{
    // sets the value from 0 to 1 back to the original value
    if (ratioToValue)
    {
        const double convertedValue = ratioToValue (index, newValue);
        const juce::ScopedValueSetter<bool> svs (ignoreCallbacks, true);
        slider.setValue (convertedValue, juce::sendNotificationSync);
    }
}

void AmatiSliderParameterAttachment::sliderValueChanged (juce::Slider*)
{
    // convert from original to 0 to 1
    if (!ignoreCallbacks)
    {
        // const auto value = slider.getValue();
        // const auto convertedValue = valueToRatio (index, value);
        // attachment.setValueAsPartOfGesture (static_cast<float> (convertedValue));
    }
}

AmatiSliderAttachment::AmatiSliderAttachment (
    const int index,
    const juce::AudioProcessorValueTreeState& stateToUse,
    const juce::String& parameterID,
    juce::Slider& slider,
    const std::function<double (int, double)>& valueToRatio,
    const std::function<double (int, double)>& ratioToValue)
{
    // TODO: Create a new type that inherits RangedAudioParameter and override the convertFrom/To0to1 functions with faust
    if (juce::RangedAudioParameter* parameter = stateToUse.getParameter (parameterID); parameter && valueToRatio && ratioToValue)
    {
        const auto initValue = static_cast<float> (valueToRatio (index, slider.getValue()));
        parameter->setValueNotifyingHost (initValue);
        attachment = std::make_unique<AmatiSliderParameterAttachment> (
            index,
            *parameter,
            slider,
            valueToRatio,
            ratioToValue,
            stateToUse.undoManager);
    }
    else
    {
        jassertfalse;
    }
}

AmatiSliderAttachment::AmatiSliderAttachment (const juce::AudioProcessorValueTreeState& value_tree_state, const juce::String& id, const juce::Slider& slider, const std::shared_ptr<ValueConverter> fConversion)
{

}
