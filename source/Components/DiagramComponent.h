#pragma once
#include "juce_gui_basics/juce_gui_basics.h"
#include <faust/dsp/libfaust.h>

class DiagramComponent final : public juce::Component
{
public:
    DiagramComponent();
    ~DiagramComponent();

    void setSource(juce::String source);
    void paint (juce::Graphics&) override;
    void resized() override;
private:
   std::unique_ptr<juce::Drawable> svg;
   juce::String path;
};

