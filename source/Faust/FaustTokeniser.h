#pragma once
#include "juce_gui_extra/juce_gui_extra.h"
class FaustTokeniser   : public juce::CodeTokeniser
{
public:
    //==============================================================================
    FaustTokeniser();
    ~FaustTokeniser() override;

    //==============================================================================
    int readNextToken (juce::CodeDocument::Iterator&) override;
    juce::CodeEditorComponent::ColourScheme getDefaultColourScheme() override;

    /** The token values returned by this tokeniser. */
    enum TokenType
    {
        tokenType_error = 0,
        tokenType_comment,
        tokenType_primitive,
        tokenType_operator,
        tokenType_identifier,
        tokenType_integer,
        tokenType_float,
        tokenType_string,
        tokenType_bracket,
        tokenType_punctuation
    };

private:

    juce::Colour oneDarkBlack = juce::Colour(40, 44, 52);
    juce::Colour oneDarkDeepGrey = juce::Colour(50, 54, 62);
    juce::Colour oneDarkMidGrey = juce::Colour(92, 99, 112);
    juce::Colour oneDarkLightGrey = juce::Colour(171,178, 191);
    juce::Colour oneDarkPurple = juce::Colour(198, 120, 221);
    juce::Colour oneDarkRed = juce::Colour(224, 108, 117);
    juce::Colour oneDarkGreen = juce::Colour(152, 195, 121);
    juce::Colour oneDarkTeal = juce::Colour(86, 182, 194);
    juce::Colour oneDarkBlue = juce::Colour(97, 175, 239);
    juce::Colour oneDarkYellow = juce::Colour(229, 192, 123);
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FaustTokeniser)
};


