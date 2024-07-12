#pragma once
#include "../AmatiLookAndFeel.h"
#include "../Faust/FaustTokeniser.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include "juce_gui_extra/juce_gui_extra.h"

class EditorComponent : public juce::Component
{
public:
    EditorComponent();

    void paint(juce::Graphics&) override {}
    void resized() override;

    juce::String getSource();
    void setSource(const juce::String&);
    std::function<void(void)> onCompile;
    std::function<void(void)> onEnable;
    std::function<void(void)> onDisable;
    void setStatus(const juce::String&, juce::NotificationType);
private:
    FaustTokeniser tokeniser;
    // Source code HAVE to be declared before codeEditor
    juce::CodeDocument sourceCode;
    juce::CodeEditorComponent codeEditor;

    juce::TextButton compileButton;
    juce::TextButton enableButton;
    juce::TextButton importButton;
    juce::TextButton exportButton;

    juce::FlexBox buttons;
    juce::Label statusLabel;

    std::unique_ptr<juce::FileChooser> fileChooser;
    juce::File workDir;
    juce::Font font;
};

