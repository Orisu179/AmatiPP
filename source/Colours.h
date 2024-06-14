#pragma once
#include <unordered_map>
#include "juce_gui_basics/juce_gui_basics.h"

// I still don't know if the colours should be a singleton, but I want to make it work first
class Colours final
{
public:
    static Colours& getInstance();
    std::unordered_map<juce::String, juce::Colour>& getCurrentColours();
private:
    Colours();
    ~Colours() = default;
    const juce::Colour oneDarkBlack = juce::Colour(40, 44, 52);
    const juce::Colour oneDarkDeepGrey= juce::Colour(50, 54, 62);
    const juce::Colour oneDarkMidGrey= juce::Colour(92,99,112);
    const juce::Colour oneDarkLightGrey = juce::Colour(171, 178, 191);
    const juce::Colour oneDarkPurple = juce::Colour(198, 120, 221);
    const juce::Colour oneDarkRed = juce::Colour(224, 108, 117);
    const juce::Colour oneDarkGreen = juce::Colour(152, 195, 121);
    const juce::Colour oneDarkTeal = juce::Colour(86, 182, 194);
    const juce::Colour oneDarkBlue = juce::Colour(97, 175, 239);
};

