#include "AmatiLookAndFeel.h"

AmatiLookAndFeel::AmatiLookAndFeel()
{
    setColour (ColourScheme::UIColour::widgetBackground, oneDarkBlack);
    setColour (juce::ResizableWindow::backgroundColourId, oneDarkBlack);
    setColour (juce::TabbedComponent::backgroundColourId, oneDarkBlack);
    setColour (juce::TabbedComponent::outlineColourId, oneDarkTeal);
    setColour (juce::TextButton::buttonColourId, oneDarkBlack);
    setColour (juce::CodeEditorComponent::backgroundColourId, oneDarkDeepGrey);
    setColour (juce::TextEditor::backgroundColourId, oneDarkBlack);
    setColour (juce::ComboBox::backgroundColourId, oneDarkBlack);
    setColour (juce::ScrollBar::thumbColourId, oneDarkGreen);
}