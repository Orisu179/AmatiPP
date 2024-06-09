#pragma once
#include "juce_gui_basics/juce_gui_basics.h"
#include "juce_gui_extra/juce_gui_extra.h"
#include "FaustTokeniser.h"
#include "AmatiLookAndFeel.h"

class EditorComponent : public juce::Component
{
public:
    EditorComponent();

    void paint(juce::Graphics&) override {}
    void resized() override;

    juce::String getSource();
    void setSource(juce::String);
    std::function<void(void)> onCompile;
    std::function<void(void)> onRevert;
private:
    FaustTokeniser tokeniser;

    // Source code HAVE to be declared before codeEditor
    juce::CodeDocument sourceCode;
    juce::CodeEditorComponent codeEditor;

    juce::TextButton compileButton;
    juce::TextButton revertButton;
    juce::TextButton importButton;
    juce::TextButton exportButton;

    std::unique_ptr<juce::FileChooser> fileChooser;
    juce::File workDir;
};

