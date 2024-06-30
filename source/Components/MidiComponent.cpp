#include "MidiComponent.h"

MidiComponent::MidiComponent() : keyboardComponent(keyboardState, juce::KeyboardComponentBase::Orientation::horizontalKeyboard)
{
    addAndMakeVisible(midiInputList);
    midiInputList.setTextWhenNoChoicesAvailable("No Midi Inputs Enabled");
    juce::StringArray midiInputNames;
    auto midiInputs = juce::MidiInput::getAvailableDevices();

    for(auto input: midiInputs){
        midiInputNames.add(input.name);
    }

    midiInputList.addItemList(midiInputNames, 1);
    midiInputList.onChange = [this]{ setMidiInput(midiInputList.getSelectedItemIndex()); };

    // use the first enabled device as the default
    for (auto input: midiInputs){
        if(deviceManager.isMidiInputDeviceEnabled(input.identifier))
        {
            setMidiInput(midiInputs.indexOf(input));
            break;
        }
    }
    // case where there's no enabled devices
    if(midiInputList.getSelectedId() == 0)
        setMidiInput(0);

    addAndMakeVisible(keyboardComponent);
    keyboardState.addListener(this);
}

void MidiComponent::handleIncomingMidiMessage (juce::MidiInput* source, const juce::MidiMessage& message)
{
    juce::ignoreUnused(source);
    const juce::ScopedValueSetter<bool> scopedInputFlag(isAddingFromMidiInput, true);
    keyboardState.processNextMidiEvent(message);
    DBG(message.getDescription());
}

// listen to selected device, while enabling it if it's disabled
void MidiComponent::setMidiInput (int index)
{
    auto list = juce::MidiInput::getAvailableDevices();
    deviceManager.removeMidiInputDeviceCallback(list[lastInputIndex].identifier, this);

    auto newInput = list[index];
    if(!deviceManager.isMidiInputDeviceEnabled(newInput.identifier))
        deviceManager.setMidiInputDeviceEnabled(newInput.identifier, true);

    deviceManager.addMidiInputDeviceCallback(newInput.identifier, this);
    midiInputList.setSelectedId(index + 1, juce::dontSendNotification);

    lastInputIndex = index;
}

void MidiComponent::handleNoteOff (juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity)
{
   if(!isAddingFromMidiInput)
   {
       auto m = juce::MidiMessage::noteOff(midiChannel, midiNoteNumber, velocity);
       m.setTimeStamp(juce::Time::getMillisecondCounterHiRes());
       DBG(m.getDescription());
   }
}

void MidiComponent::handleNoteOn (juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) {
    auto m = juce::MidiMessage::noteOn(midiChannel, midiNoteNumber, velocity);
    m.setTimeStamp(juce::Time::getMillisecondCounterHiRes());
    DBG(m.getDescription());
}

void MidiComponent::resized()
{

}
