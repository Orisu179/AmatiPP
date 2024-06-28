#pragma once
#include "juce_gui_basics/juce_gui_basics.h"
class ComboBoxSetting : public juce::Component
{
public:
    ComboBoxSetting(const juce::Value& value, const juce::String& labelText, const std::vector<juce::String>& items);

    void paint (juce::Graphics&) override {}
    void resized () override;

private:
    juce::Label label;
    juce::ComboBox comboBox;
};

class SettingsComponent : public juce::Component
{
public:
    SettingsComponent(juce::ValueTree settingsTree);

    void paint (juce::Graphics&) override {}
    void resized() override;
private:
    ComboBoxSetting backendComboBox;
//    ComboBoxSetting themeComboBox;
};