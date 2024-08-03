#pragma once
#include <faust/dsp/dsp.h>
#include <faust/gui/ValueConverter.h>
#include <memory>
#include "AmatiSliderAttachment.h"

class SliderBuilder {
public:
    explicit SliderBuilder(juce::AudioProcessorValueTreeState& vts);

    void setMetaData (const std::map<juce::String, juce::String>&);
    void setParameterData (const FaustProgram::Parameter&);
    void buildParameters (const std::vector<FaustProgram::Parameter> params);
    void reset();
    [[nodiscard]] juce::Slider* getSlider() const;
    AmatiSliderAttachment* getAttachment (juce::Slider* slider, const juce::String& id);

private:
    std::shared_ptr<ValueConverter> fConversion;
    FaustProgram::Parameter curParam;
    juce::AudioProcessorValueTreeState& valueTreeState;
    AmatiSliderAttachment* attachment;
    juce::Slider* attachedSlider;
    void buildHidden(const bool) const;
    void buildUnit(const juce::String& value) const;
    void buildScale (const juce::String& value);
    // TODO: implement the rest
    void buildTooltip();
    void buildStyle();
    void buildScreenColor();
};

