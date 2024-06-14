#include "AmatiLookAndFeel.h"

AmatiLookAndFeel::AmatiLookAndFeel() {
    setColour(juce::ResizableWindow::backgroundColourId, oneDarkBlack);
    setColour(juce::TabbedComponent::backgroundColourId, oneDarkBlack);
    setColour(juce::TabbedComponent::outlineColourId, oneDarkPurple);
    setColour(juce::TextButton::buttonColourId, oneDarkBlack);
    setColour(juce::CodeEditorComponent::backgroundColourId, oneDarkDeepGrey);
    setColour(juce::TextEditor::backgroundColourId, oneDarkBlack);
    setColour(juce::ComboBox::backgroundColourId, oneDarkBlack);
}