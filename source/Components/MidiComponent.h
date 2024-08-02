#pragma once
#include "juce_audio_devices/juce_audio_devices.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include "../Faust/FaustMidi.h"
#include "juce_audio_utils/gui/juce_KeyboardComponentBase.h"
#include "juce_audio_utils/gui/juce_MidiKeyboardComponent.h"
class MidiComponent final : public juce::Component,
                            protected juce::MidiInputCallback,
                            juce::MidiKeyboardState::Listener
{
public:
    explicit MidiComponent ();
    void handleIncomingMidiMessage (juce::MidiInput* source, const juce::MidiMessage& message) override;
    void handleNoteOn (juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;
    void handleNoteOff (juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;
    void setHandleMidiHandler (const std::function<void(juce::MidiMessage&)>&);

    void paint (juce::Graphics&) override {}
    void resized() override;

private:
    void setMidiInput (int);
    juce::AudioDeviceManager deviceManager;
    juce::ComboBox midiInputList;
    juce::Label midiInputListLabel;

    std::function<void (juce::MidiMessage&)> handleMidi;
    juce::FlexBox inputList;
    int lastInputIndex;
    bool isAddingFromMidiInput;

    juce::MidiKeyboardState keyboardState;
    juce::MidiKeyboardComponent keyboardComponent;
};
