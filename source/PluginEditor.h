#pragma once

#include "AmatiLookAndFeel.h"
#include "BinaryData.h"
#include "Components/ConsoleComponent.h"
#include "Components/EditorComponent.h"
#include "Components/SettingsComponent.h"
#include "Components/MidiComponent.h"
#include "ParamEditor.h"
#include "PluginProcessor.h"
#include "melatonin_inspector/melatonin_inspector.h"

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
    PluginProcessor& processorRef;
    juce::AudioProcessorValueTreeState& valueTreeState;
    juce::ValueTree settingTree;
    AmatiLookAndFeel amatiLookAndFeel;

    void updateParameters();
    void updateEditor();

    SettingsComponent settingsComponent;
    ConsoleComponent consoleComponent;
    EditorComponent editorComponent;
    MidiComponent midiComponent;
    ParamEditor paramEditor;
    juce::TabbedComponent tabbedComponent;

    // melatonin stuff
    std::unique_ptr<melatonin::Inspector> inspector;
    juce::TextButton inspectButton { "Inspect the UI" };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
