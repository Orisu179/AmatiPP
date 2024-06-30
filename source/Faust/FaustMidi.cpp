#include "FaustMidi.h"


bool FaustMidi::checkMidi (juce::String source) {
   return (source.contains("declare options \"[midi:on]\";"));
}