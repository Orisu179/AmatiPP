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
    fConversion = nullptr;
    attachedSlider = nullptr;
}

juce::Slider* SliderBuilder::getSlider() const
{
    jassert(attachedSlider);
    return attachedSlider;
}

AmatiSliderAttachment* SliderBuilder::getAttachment (juce::Slider* slider, const juce::String& id)
{
    attachment = new AmatiSliderAttachment (valueTreeState, id, *slider, fConversion);
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

void SliderBuilder::buildScale (const juce::String& value = "linear")
{
    if(value == "linear")
    {
        const auto temp = std::make_shared<LinearValueConverter> (curParam.range.getStart(), curParam.range.getEnd(), 0.0, 1.0);
        fConversion = std::dynamic_pointer_cast<LinearValueConverter> (temp);
    }
   if(value == "log")
   {
       // const auto temp = std::make_shared<LogValueConverter>(curParam.range.getStart(), curParam.range.getEnd(), 0.0, 0.1);
       // fConversion = std::dynamic_pointer_cast<LogValueConverter> (temp);
        const auto temp = std::make_shared<LinearValueConverter> (curParam.range.getStart(), curParam.range.getEnd(), 0.0, 1.0);
        fConversion = std::dynamic_pointer_cast<LinearValueConverter> (temp);
        const double min = curParam.range.getStart();
        const double max = curParam.range.getEnd();
        const double mid = std::sqrt(max * min);
        const double skewFactor = std::log2(0.5) / std::log2((mid - min) / (max - min));
        attachedSlider->setSkewFactor (skewFactor);
   } else if (value == "exp")
   {
       const auto temp = std::make_shared<ExpValueConverter>(curParam.range.getStart(), curParam.range.getEnd(), 0.0, 0.1);
       fConversion = std::dynamic_pointer_cast<ExpValueConverter> (temp);
   }
}

void SliderBuilder::setMetaData (const std::map<juce::String, juce::String>& metaData)
{
    if (metaData.empty())
    {
        buildScale ();
    } else
    {
        for (const auto& [key, value] : metaData)
        {
            if(key == "scale")
            {
               buildScale (value);
            } else
            {
                buildScale ();
            }

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
}
void SliderBuilder::buildParameters (const std::vector<FaustProgram::Parameter> params)
{

}
