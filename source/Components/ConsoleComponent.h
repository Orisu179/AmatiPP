#pragma once
#include "juce_gui_basics/juce_gui_basics.h"

class ConsoleComponent final : public juce::Component,
                         public juce::Logger
{
public:
    ConsoleComponent();
    ~ConsoleComponent() override;

    void paint (juce::Graphics&) override {}
    void resized() override;

    void clearMessages();
    void logMessage (const juce::String&) override;

private:
    juce::TextEditor console;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ConsoleComponent)
};