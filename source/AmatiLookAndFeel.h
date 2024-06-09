#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
class AmatiLookAndFeel : public juce::LookAndFeel_V4
{
public:
    //==============================================================================
    AmatiLookAndFeel();
private:
    juce::Colour oneDarkBlack = juce::Colour(40, 44, 52);
    juce::Colour oneDarkDeepGrey= juce::Colour(50, 54, 62);
    juce::Colour oneDarkMidGrey= juce::Colour(92,99,112);
    juce::Colour oneDarkLightGrey = juce::Colour(171, 178, 191);
    juce::Colour oneDarkPurple = juce::Colour(198, 120, 221);
    juce::Colour oneDarkRed = juce::Colour(224, 108, 117);
    juce::Colour oneDarkGreen = juce::Colour(152, 195, 121);
    juce::Colour oneDarkTeal = juce::Colour(86, 182, 194);
    juce::Colour oneDarkBlue = juce::Colour(97, 175, 239);

};

