#include "AmatiLookAndFeel.h"

AmatiLookAndFeel::AmatiLookAndFeel() {
    setColour(ColourScheme::UIColour::widgetBackground, globalColours.getColour("oneDarkBlack"));
    setColour(juce::ResizableWindow::backgroundColourId, globalColours.getColour("oneDarkBlack"));
    setColour(juce::TabbedComponent::backgroundColourId, globalColours.getColour("oneDarkBlack"));
    setColour(juce::TabbedComponent::outlineColourId, globalColours.getColour("oneDarkPurple"));
    setColour(juce::TextButton::buttonColourId, globalColours.getColour("oneDarkBlack"));
    setColour(juce::CodeEditorComponent::backgroundColourId, globalColours.getColour("oneDarkDeepGrey"));
    setColour(juce::TextEditor::backgroundColourId, globalColours.getColour("oneDarkBlack"));
    setColour(juce::ComboBox::backgroundColourId, globalColours.getColour("oneDarkBlack"));
}