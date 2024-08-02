#pragma once
#include "../AmatiLookAndFeel.h"
#include "../Faust/FaustTokeniser.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include "juce_gui_extra/juce_gui_extra.h"

class EditorComponent : public juce::Component
{
public:
    EditorComponent();

    void paint (juce::Graphics&) override {}
    void resized() override;

    juce::String getSource() const;
    void setSource (const juce::String&);
    std::function<void ()> onCompile;
    std::function<void ()> onStart;
    std::function<void ()> onStop;
    void setStatus (const juce::String&, juce::NotificationType);
    void setStartButtonEnabled (bool);

private:
    FaustTokeniser tokeniser;
    // Source code HAVE to be declared before codeEditor
    juce::CodeDocument sourceCode;
    juce::CodeEditorComponent codeEditor;

    juce::TextButton compileButton;
    juce::TextButton startButton;
    juce::TextButton importButton;
    juce::TextButton exportButton;

    juce::FlexBox buttons;
    juce::Label statusLabel;

    std::unique_ptr<juce::FileChooser> fileChooser;
    juce::File workDir;
    juce::Font font;
};
