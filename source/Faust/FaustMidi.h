#pragma once
#include "FaustProgram.h"
#include "faust/midi/midi.h"

class FaustMidi
{
public:
    FaustMidi();

    bool checkMidi(juce::String);
//    void handleIncomingMidiMessages(juce::MidiInpu) override
private:
   FaustProgram faustProgram;
};
