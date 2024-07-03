#pragma once
#include "faust/midi/midi.h"
#include "juce_core/juce_core.h"
#include "juce_audio_basics/juce_audio_basics.h"
#include "juce_audio_devices/juce_audio_devices.h"

class FAUST_API MapUI;

class FaustMidi : public midi_handler, juce::MidiInputCallback
{
public:
    FaustMidi();
    virtual ~FaustMidi();

    static bool checkMidi(const juce::String&);
    void encodeBuffer(juce::MidiBuffer&);
    void decodeBuffer(juce::MidiBuffer&);
    bool startMidi() override;
    void stopMidi() override;

    MapUI* keyOn(double, int channel, int pitch, int velocity) override;
    void keyOff(double, int channel, int pitch, int velocity) override;
    void ctrlChange(int channel, int ctrl, int value) override;
    void chanPress(int channel, int press) override;
    void progChange(int channel, int pgm) override;
    void keyPress(int channel, int pitch, int press) override;
    void pitchWheel(double, int channel, int wheel) override;
    void ctrlChange14bits(double, int channel, int ctrl, int value) override;
    void startSync(double date) override;
    void stopSync(double date) override;
    void clock(double date) override;
    void sysEx(double, std::vector<unsigned char> &message) override;

private:
    std::unique_ptr<juce::MidiInput> fMidiIn;
    std::unique_ptr<juce::MidiOutput> fMidiOut;
    juce::MidiBuffer fOutputBuffer;
    juce::CriticalSection fMutex;
    void decodeMessages(const juce::MidiMessage& message);
    void handleIncomingMidiMessage(juce::MidiInput *source, const juce::MidiMessage &message) override;
};
