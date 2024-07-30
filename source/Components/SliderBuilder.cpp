#include "SliderBuilder.h"

SliderBuilder::SliderBuilder (juce::AudioProcessorValueTreeState& vts) : curParam(), valueTreeState (vts), attachment (nullptr), attachedSlider (new juce::Slider())
{
}

void SliderBuilder::setParameterData (const FaustProgram::Parameter& param)
{
    curParam = param;
    setMetaData (curParam.metaData);
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
    if(!attachedSlider)
    {
        DBG("not initalized");
        jassertfalse;
    }
    // auto* slider = new juce::Slider();
    // slider->setNormalisableRange (range);
    // slider->setValue (curParam.init, juce::dontSendNotification);
    return attachedSlider;
}

AmatiSliderAttachment* SliderBuilder::getAttachment (const juce::Slider* slider, const juce::String& id)
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

void SliderBuilder::buildScale (const juce::String& value)
{
   if(value == "log")
   {
       fConversion = std::make_shared<LogValueConverter>(curParam.range.getStart(), curParam.range.getEnd(), curParam.range.getStart(), curParam.range.getEnd());
       attachedSlider->setSkewFactor(0.2f);
   } else if (value == "exp")
   {
       fConversion = std::make_shared<ExpValueConverter>(curParam.range.getStart(), curParam.range.getEnd(), curParam.range.getStart(), curParam.range.getEnd());
   }
}

void SliderBuilder::setMetaData (const std::map<juce::String, juce::String>& metaData)
{
    reset();
    attachedSlider = new juce::Slider();
    const auto range = juce::NormalisableRange (curParam.range, curParam.step);
    attachedSlider->setNormalisableRange (range);
    attachedSlider->setValue (curParam.init, juce::dontSendNotification);
    for (const auto& [key, value] : metaData)
    {
        if(key == "scale")
        {
           buildScale (value);
        } else
        {
            fConversion = std::make_shared<LinearValueConverter> (curParam.range.getStart(), curParam.range.getEnd(), curParam.range.getStart(), curParam.range.getEnd());
        }

        if(key == "unit")
        {
            buildUnit(value);
        }
        if(key == "hidden")
        {
            buildHidden (true);
        }
        // switch (key)
        // {
            // case "scale":
            //     buildScale (value);
            // case "unit":
            //     buildUnit();
            // case "hidden":
            //     buildHidden();
            // case "tooltip":
            //     buildTooltip();
            // case "style":
            //     buildStyle();
            // case "acc":
            //     buildAcc();
            // case "gyr":
            //     buildGyr();
            // case "screencolor":
            //     buildScreenColor();
            // case "midi":
            // default:
            //     break;
        // }
    }
}
