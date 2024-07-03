#pragma once
#include "juce_gui_basics/juce_gui_basics.h"
#include "juce_audio_devices/juce_audio_devices.h"
//#include "juce_audio_devices/midi_io/juce_MidiDevices.h"
#include "../Faust/FaustMidi.h"
#include "juce_audio_utils/gui/juce_KeyboardComponentBase.h"
#include "juce_audio_utils/gui/juce_MidiKeyboardComponent.h"
class MidiComponent : public juce::Component,
                      protected juce::MidiInputCallback, juce::MidiKeyboardState::Listener
{
public:
    MidiComponent();
    void handleIncomingMidiMessage(juce::MidiInput *source, const juce::MidiMessage &message) override;
    void handleNoteOn(juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;
    void handleNoteOff(juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;


    void paint (juce::Graphics&) override {}
    void resized () override;
private:
    void setMidiInput(int);
    juce::AudioDeviceManager deviceManager;
    juce::ComboBox midiInputList;
    juce::Label midiInputListLabel;

    juce::FlexBox inputList;
    int lastInputIndex;
    bool isAddingFromMidiInput;

    juce::MidiKeyboardState keyboardState;
    juce::MidiKeyboardComponent keyboardComponent;
};

