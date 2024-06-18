#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include "Colours.h"
class AmatiLookAndFeel : public juce::LookAndFeel_V4
{
public:
    //==============================================================================
    AmatiLookAndFeel();
private:
    Colours& globalColours = Colours::getInstance();
};

