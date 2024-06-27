#pragma once
#include "juce_gui_basics/juce_gui_basics.h"
#include "juce_audio_devices/juce_audio_devices.h"
//#include "juce_audio_devices/midi_io/juce_MidiDevices.h"
#include "juce_audio_utils/gui/juce_KeyboardComponentBase.h"
#include "juce_audio_utils/gui/juce_MidiKeyboardComponent.h"
#include "../FaustMidi.h"
class MidiComponent : juce::Component, juce::MidiInputCallback, juce::MidiKeyboardState::Listener
{
public:
    MidiComponent();
    void handleIncomingMidiMessage(juce::MidiInput *source, const juce::MidiMessage &message) override;
private:
    FaustMidi faustMidi;

    void setMidiInput(int);
    juce::AudioDeviceManager deviceManager;
    juce::ComboBox midiInputList;
    juce::Label midiInputListLabel;

    int lastInputIndex;
    bool isAddingFromMidiInput;

    juce::MidiKeyboardState keyboardState;
    juce::MidiKeyboardComponent keyboardComponent;
};

