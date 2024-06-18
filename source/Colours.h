#pragma once
#include <unordered_map>
#include "juce_gui_basics/juce_gui_basics.h"

// I still don't know if the colours should be a singleton, but I want to make it work first
// So here is a meyers singleton implementation
class Colours final
{
public:
    static Colours& getInstance();

    juce::Colour getColour(const juce::String&);

    juce::LookAndFeel_V4::ColourScheme getOneDarkColourScheme();
private:
    // Don't allow public constructors and destructors
    Colours() = default;
    ~Colours() = default;

    // disable copy and assignment
    Colours(Colours const&) = delete;
    Colours& operator = (Colours const&) = delete;
    Colours(Colours&&) = delete;
    Colours& operator = (Colours&&) = delete;

    const std::unordered_map<juce::String, juce::Colour> oneDark {
        {"oneDarkBlack", juce::Colour(40, 44, 52)},
        {"oneDarkDeepGrey", juce::Colour(50, 54, 62)},
        {"oneDarkMidGrey", juce::Colour(92, 99, 112)},
        {"oneDarkLightGrey", juce::Colour(171,178, 191)},
        {"oneDarkPurple", juce::Colour(198, 120, 221)},
        {"oneDarkRed", juce::Colour(224, 108, 117)},
        {"oneDarkGreen", juce::Colour(152, 195, 121)},
        {"oneDarkTeal", juce::Colour(86, 182, 194)},
        {"oneDarkBlue", juce::Colour(97, 175, 239)},
        {"oneDarkYellow", juce::Colour(229, 192, 123)}
    };

    // In the future, colours could be added with maps

};

