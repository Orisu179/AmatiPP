#include "Colours.h"

Colours& Colours::getInstance() {
   static Colours colours;
   return colours;
}

juce::Colour Colours::getColour(const juce::String& colour) {
    if(!oneDark.contains(colour)){
        return oneDark.at("oneDarkLightGrey");
    }
    return oneDark.at(colour);
}

//juce::LookAndFeel_V4::ColourScheme Colours::getOneDarkColourScheme() {
//    return {};
//}
