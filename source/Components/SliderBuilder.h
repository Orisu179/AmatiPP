#pragma once
#include "ParamEditor.h"
#include <faust/gui/ValueConverter.h>
#include <memory>

class SliderBuilder {
public:
    SliderBuilder(juce::AudioProcessorValueTreeState& vts);

    void setMetaData (const std::map<juce::String, juce::String>&);
    void setParameterData (const FaustProgram::Parameter&);
    void reset();
    juce::Slider* getSlider() const;
    AmatiSliderAttachment* getAttachment (const juce::Slider* slider, const juce::String& id);
    enum scale
    {
        log,
        exp
    };

private:
    ValueConverter* fConversion;
    FaustProgram::Parameter curParam;
    juce::AudioProcessorValueTreeState& valueTreeState;
    void buildTooltip();
    void buildHidden();
    void buildUnit();
    void buildScale(scale value);
    void buildStyle();
    void buildAcc();
    void buildGyr();
    void buildScreenColor();
};

