#include "SliderBuilder.h"

SliderBuilder::SliderBuilder (juce::AudioProcessorValueTreeState& vts) : valueTreeState (vts)
{

}

void SliderBuilder::setMetaData (const std::map<juce::String, juce::String>& metaData)
{
    for (const auto& [key, value] : metaData)
    {
        switch (key)
        {
            case "scale":
                break;
            case "unit":
                break;
            case "hidden":
                break;
            case "tooltip":
                break;
            case "style":
                break;
            case "acc":
                break;
            case "gyr":
                break;
            case "screencolor":
                break;
            case "midi":
            default:
                break;
        }
    }
}
void SliderBuilder::setParameterData (const FaustProgram::Parameter& param)
{
    curParam = param;
    setMetaData (curParam.metaData);
}

void SliderBuilder::reset()
{
    curParam = {};
}

juce::Slider* SliderBuilder::getSlider() const
{
    auto* slider = new juce::Slider();
    const auto range = juce::NormalisableRange (curParam.range, curParam.step);
    slider->setNormalisableRange (range);
    slider->setValue (curParam.init, juce::dontSendNotification);
    return slider;
}

AmatiSliderAttachment* SliderBuilder::getAttachment (const juce::Slider* slider, const juce::String& id)
{
    auto* attachment = new AmatiSliderAttachment (valueTreeState, id, *slider, fConversion->faust2ui, fConversion->ui2faust);
    reset();
    return attachment;
}

void SliderBuilder::buildTooltip()
{
}
void SliderBuilder::buildHidden()
{
}
void SliderBuilder::buildUnit()
{
}
void SliderBuilder::buildScale (scale value)
{
   if(value == log)
   {

   } else if (value == exp)
   {

   }
}

