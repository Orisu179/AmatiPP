#pragma once

#include "PluginProcessor.h"
#include "BinaryData.h"
#include "melatonin_inspector/melatonin_inspector.h"
#include "ConsoleComponent.h"
#include "SettingsComponent.h"
#include "ParamEditor.h"
#include "EditorComponent.h"
#include <faust/gui/MapUI.h>

//==============================================================================
class PluginEditor : public juce::AudioProcessorEditor,
                     public juce::ValueTree::Listener
{
public:
    explicit PluginEditor (PluginProcessor& p, juce::AudioProcessorValueTreeState& vts);
    ~PluginEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PluginProcessor& processorRef;
    juce::AudioProcessorValueTreeState& valueTreeState;
    juce::ValueTree settingTree;

    MapUI faustUI;
    void updateParameters();
    void updateEditor();

    SettingsComponent settingsComponent;
    ConsoleComponent consoleComponent;
    EditorComponent editorComponent;
    ParamEditor paramEditor;
    juce::TabbedComponent tabbedComponent;
    juce::Label statusLabel;

    // melatonin stuff
    std::unique_ptr<melatonin::Inspector> inspector;
    juce::TextButton inspectButton { "Inspect the UI" };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
