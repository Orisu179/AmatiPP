#include "MidiComponent.h"

MidiComponent::MidiComponent () : midiInputListLabel ("MIDI Input"), keyboardComponent (keyboardState, juce::KeyboardComponentBase::Orientation::horizontalKeyboard)
{
    addAndMakeVisible (midiInputList);
    midiInputList.setTextWhenNoChoicesAvailable ("No Midi Inputs Enabled");
    juce::StringArray midiInputNames;
    auto midiInputs = juce::MidiInput::getAvailableDevices();

    for (auto input : midiInputs)
    {
        midiInputNames.add (input.name);
    }

    midiInputList.addItemList (midiInputNames, 1);
    midiInputList.onChange = [this] { setMidiInput (midiInputList.getSelectedItemIndex()); };

    midiInputListLabel.setText ("MIDI Input: ", juce::dontSendNotification);
    midiInputListLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    // use the first enabled device as the default
    for (auto input : midiInputs)
    {
        if (deviceManager.isMidiInputDeviceEnabled (input.identifier))
        {
            setMidiInput (midiInputs.indexOf (input));
            break;
        }
    }
    // case where there's no enabled devices
    if (midiInputList.getSelectedId() == 0)
        setMidiInput (0);

    addAndMakeVisible (midiInputListLabel);
    addAndMakeVisible (keyboardComponent);
    keyboardState.addListener (this);
}

void MidiComponent::handleIncomingMidiMessage (juce::MidiInput* source, const juce::MidiMessage& message)
{
    juce::ignoreUnused (source);
    const juce::ScopedValueSetter<bool> scopedInputFlag (isAddingFromMidiInput, true);
    keyboardState.processNextMidiEvent (message);
}

// listen to selected device, while enabling it if it's disabled
void MidiComponent::setMidiInput (int index)
{
    const auto list = juce::MidiInput::getAvailableDevices();
    deviceManager.removeMidiInputDeviceCallback (list[lastInputIndex].identifier, this);
    const auto newInput = list[index];
    if (!deviceManager.isMidiInputDeviceEnabled (newInput.identifier))
        deviceManager.setMidiInputDeviceEnabled (newInput.identifier, true);

    deviceManager.addMidiInputDeviceCallback (newInput.identifier, this);
    midiInputList.setSelectedId (index + 1, juce::dontSendNotification);

    lastInputIndex = index;
}

void MidiComponent::handleNoteOff (juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity)
{
    if (!isAddingFromMidiInput && handleMidi)
    {
        auto m = juce::MidiMessage::noteOff (midiChannel, midiNoteNumber, velocity);
        m.setTimeStamp (juce::Time::getMillisecondCounterHiRes());
        handleMidi (m);
    }
}
void MidiComponent::setHandleMidiHandler (const std::function<void (juce::MidiMessage&)>& midiHandler)
{
    if(midiHandler)
    {
        handleMidi = midiHandler;
    } else
    {
        jassertfalse;
    }
}

void MidiComponent::handleNoteOn (juce::MidiKeyboardState*, const int midiChannel, const int midiNoteNumber, const float velocity)
{
    if(handleMidi)
    {
        auto m = juce::MidiMessage::noteOn (midiChannel, midiNoteNumber, velocity);
        m.setTimeStamp (juce::Time::getMillisecondCounterHiRes());
        handleMidi(m);
    }
}

void MidiComponent::resized()
{
    constexpr int margin = 10;
    using fb = juce::FlexBox;
    inputList.flexDirection = fb::Direction::row;
    inputList.alignItems = fb::AlignItems::center;
    inputList.flexWrap = fb::Wrap::noWrap;
    inputList.justifyContent = fb::JustifyContent::center;

    inputList.items.add (juce::FlexItem (midiInputListLabel).withMinHeight (60).withMinWidth (100));
    inputList.items.add (juce::FlexItem (midiInputList).withMargin (static_cast<float> (margin)).withMinHeight (30).withMinWidth (getWidth() / 2));

    // midiInputList.setBounds(getWidth()/4, margin, getWidth()/2, margin*2);
    inputList.performLayout (getLocalBounds().removeFromTop (margin * 4));
    keyboardComponent.setBounds (margin, margin * 4, getWidth() - margin * 2, getHeight() - margin * 8);
}
