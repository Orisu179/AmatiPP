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
    void setSource(const juce::String&);
    std::function<void(void)> onCompile;
    std::function<void(void)> onRevert;
    void setStatus(const juce::String&, juce::NotificationType);
private:
    FaustTokeniser tokeniser;

    // Source code HAVE to be declared before codeEditor
    juce::CodeDocument sourceCode;
    juce::CodeEditorComponent codeEditor;

    juce::TextButton compileButton;
    juce::TextButton revertButton;
    juce::TextButton importButton;
    juce::TextButton exportButton;

    juce::FlexBox buttons;
    juce::FlexBox topBar;

    juce::Label statusLabel;

    std::unique_ptr<juce::FileChooser> fileChooser;
    juce::File workDir;
};

