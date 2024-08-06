#include "SliderBuilder.h"

SliderBuilder::SliderBuilder (juce::AudioProcessorValueTreeState& vts) : curParam(), valueTreeState (vts), attachment (nullptr)
{

}

void SliderBuilder::setParameterData (const FaustProgram::Parameter& param)
{
    curParam = param;
    attachedSlider = new juce::Slider();
    const auto range = juce::NormalisableRange (param.range, param.step);
    attachedSlider->setNormalisableRange (range);
    attachedSlider->setValue (param.init, juce::dontSendNotification);
    setMetaData (param.metaData);
}

void SliderBuilder::reset()
{
    curParam = {};
    attachment = nullptr;
    attachedSlider = nullptr;
}

juce::Slider* SliderBuilder::getSlider() const
{
    jassert(attachedSlider);
    return attachedSlider;
}

AmatiSliderAttachment* SliderBuilder::getAttachment (juce::Slider* slider, const juce::String& id)
{
    attachment =  new AmatiSliderAttachment (curParam, valueTreeState, id, *slider);
    return attachment;
}

void SliderBuilder::buildHidden (const bool hidden) const
{
    attachedSlider->setVisible (hidden);
}

void SliderBuilder::buildUnit (const juce::String& value) const
{
   attachedSlider->setTextValueSuffix (" " + value);
}

void SliderBuilder::buildScale (const juce::String& value)
{
   if(value == "log")
   {
        const double min = curParam.range.getStart();
        const double max = curParam.range.getEnd();
        const double mid = std::sqrt(max * min);
        const double skewFactor = std::log2(0.5) / std::log2((mid - min) / (max - min));
        attachedSlider->setSkewFactor (skewFactor);
   } else if (value == "exp")
   {
   }
}

void SliderBuilder::setMetaData (const std::map<juce::String, juce::String>& metaData)
{
    for (const auto& [key, value] : metaData)
    {
//            if(key == "scale")
//            {
//               buildScale (value);
//            } else
//            {
//                buildScale ();
//            }

        if(key == "unit")
        {
            buildUnit(value);
        }
        if(key == "hidden")
        {
            buildHidden (true);
        }
    }
}