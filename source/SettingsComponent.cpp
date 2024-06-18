#include "SettingsComponent.h"

SettingsComponent::SettingsComponent (juce::ValueTree settingsTree) : backendComboBox (settingsTree.getPropertyAsValue ("backend", nullptr), "Backend", { "LLVM", "Interpreter" }),
                                                                      themeComboBox(settingsTree.getPropertyAsValue("colour", nullptr), "Colour Theme", {"One Dark", "Moonlight"})
{
    addAndMakeVisible (backendComboBox);
}

void SettingsComponent::resized()
{
    using FB = juce::FlexBox;
    FB box;
    box.alignContent = FB::AlignContent::flexStart;
    box.alignItems = FB::AlignItems::flexStart;
    box.flexDirection = FB::Direction::column;
    box.flexWrap = FB::Wrap::noWrap;
    box.justifyContent = FB::JustifyContent::flexStart;

    auto addItem = [&] (auto& component) {
        auto item = juce::FlexItem (component).withMargin (10).withMaxHeight (60).withMinWidth (getLocalBounds().getWidth() - 20).withFlex (1);
        box.items.add (item);
    };
    addItem (backendComboBox);

    box.performLayout (getLocalBounds());
}

ComboBoxSetting::ComboBoxSetting (const juce::Value& value,
    const juce::String& labelText,
    const std::vector<juce::String>& items)
{
    label.setText (labelText, juce::dontSendNotification);
    label.attachToComponent (&comboBox, false);
    addAndMakeVisible (label);

    auto id = 1;
    for (const auto& item : items)
    {
        comboBox.addItem (item, id);
        ++id;
    }
    addAndMakeVisible (comboBox);

    comboBox.getSelectedIdAsValue().referTo (value);
    if (auto var = value.getValue(); var.isVoid() || static_cast<int> (var) < 1 || static_cast<int> (var) > id)
    {
        comboBox.setSelectedId (1, juce::sendNotification);
    }
}

void ComboBoxSetting::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop (label.getHeight());
    comboBox.setBounds (bounds);
}