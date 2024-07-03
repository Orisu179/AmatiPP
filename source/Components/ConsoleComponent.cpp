#include "ConsoleComponent.h"

ConsoleComponent::ConsoleComponent()
{
    console.setReadOnly (true);
    console.setCaretVisible (false);
    console.setMultiLine (true);

    // Use monospaced font
    juce::Font font;
    font.setTypefaceName (juce::Font::getDefaultMonospacedFontName ());
    console.setFont (font);

    addAndMakeVisible (&console);
}

ConsoleComponent::~ConsoleComponent()
{
   juce::Logger::setCurrentLogger(nullptr);
}

void ConsoleComponent::resized ()
{
    int margin = 10;

    console.setBounds
        (
            margin,
            margin,
            getWidth() - 2*margin,
            getHeight() - 2*margin
        );
}

void ConsoleComponent::clearMessages() {
    console.clear();
}

void ConsoleComponent::logMessage (const juce::String& message)
{
    console.setCaretPosition (console.getTotalNumChars ());
    console.insertTextAtCaret (message + "\n");
}