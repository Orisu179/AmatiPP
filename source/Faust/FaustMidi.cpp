#include "FaustMidi.h"

FaustMidi::FaustMidi() : midi_handler("JUCE") {}

FaustMidi::~FaustMidi() {
   stopMidi();
}

bool FaustMidi::checkMidi (const juce::String& source) {
   return (source.contains("declare options \"[midi:on]\";"));
}

void FaustMidi::encodeBuffer(juce::MidiBuffer & buffer) {
   const juce::ScopedTryLock lock(fMutex);
   if(lock.isLocked()) {
     buffer.swapWith(fOutputBuffer);
      fOutputBuffer.clear();
   } else {
      DBG("encodeBuffer fails...");
      jassertfalse;
   }
}

void FaustMidi::decodeBuffer(juce::MidiBuffer & buffer) {
  juce::MidiMessage msg;
   int ignore;
   for(juce::MidiBuffer::Iterator it(buffer); it.getNextEvent(msg, ignore);)
   {
      decodeMessages(msg);
   }
   buffer.clear();
}

bool FaustMidi::startMidi() {
   if((fMidiIn = juce::MidiInput::openDevice(juce::MidiInput::getDefaultDeviceIndex(), this)) == nullptr) {
      return false;
   }
   if((fMidiOut = juce::MidiOutput::openDevice(juce::MidiOutput::getDefaultDeviceIndex())) == nullptr) {
      return false;
   }
   fMidiIn->start();
   return true;
}

void FaustMidi::stopMidi() {
   fMidiIn->stop();
}

MapUI * FaustMidi::keyOn(double x, int channel, int pitch, int velocity) {
   fOutputBuffer.addEvent(juce::MidiMessage::noteOn(channel + 1, pitch, juce::uint8(velocity)), 0);
   fMidiOut->sendMessageNow(juce::MidiMessage::noteOn(channel + 1, pitch, juce::uint8(velocity)));
   return nullptr;
}

void FaustMidi::keyOff(double x, int channel, int pitch, int velocity) {
   fOutputBuffer.addEvent(juce::MidiMessage::noteOff(channel + 1, pitch, juce::uint8(velocity)), 0);
   fMidiOut->sendMessageNow(juce::MidiMessage::noteOff(channel + 1, pitch, juce::uint8(velocity)));
}

void FaustMidi::ctrlChange(int channel, int ctrl, int value) {
   fOutputBuffer.addEvent(juce::MidiMessage::controllerEvent(channel + 1, ctrl, juce::uint8(value)), 0);
   fMidiOut->sendMessageNow(juce::MidiMessage::controllerEvent(channel + 1, ctrl, juce::uint8(value)));
}

void FaustMidi::chanPress(int channel, int press) {
   fOutputBuffer.addEvent(juce::MidiMessage::channelPressureChange(channel + 1, press), 0);
   fMidiOut->sendMessageNow(juce::MidiMessage::channelPressureChange(channel + 1, press));
}

void FaustMidi::progChange(int channel, int pgm) {
   fOutputBuffer.addEvent(juce::MidiMessage::programChange(channel + 1, pgm), 0);
   fMidiOut->sendMessageNow(juce::MidiMessage::programChange(channel + 1, pgm));
}

void FaustMidi::keyPress(int channel, int pitch, int press) {
   fOutputBuffer.addEvent(juce::MidiMessage::aftertouchChange(channel + 1, pitch, press), 0);
   fMidiOut->sendMessageNow(juce::MidiMessage::aftertouchChange(channel + 1, pitch, press));
}

void FaustMidi::pitchWheel(double x, int channel, int wheel) {
   fOutputBuffer.addEvent(juce::MidiMessage::pitchWheel(channel + 1, range(0, 16383, wheel)), 0);
   fMidiOut->sendMessageNow(juce::MidiMessage::pitchWheel(channel + 1, range(0, 16383, wheel)));
}

void FaustMidi::ctrlChange14bits(double x, int channel, int ctrl, int value) {
   // TODO
}

void FaustMidi::startSync(double) {
   fOutputBuffer.addEvent(juce::MidiMessage::midiStart(), 0);
   fMidiOut->sendMessageNow(juce::MidiMessage::midiStart());
}

void FaustMidi::stopSync(double) {
   fOutputBuffer.addEvent(juce::MidiMessage::midiStop(), 0);
   fMidiOut->sendMessageNow(juce::MidiMessage::midiStop());
}

void FaustMidi::clock(double) {
   fOutputBuffer.addEvent(juce::MidiMessage::midiClock(), 0);
   fMidiOut->sendMessageNow(juce::MidiMessage::midiClock());
}

void FaustMidi::sysEx(double, std::vector<unsigned char> &message) {
   fOutputBuffer.addEvent(juce::MidiMessage(message.data(), static_cast<int>(message.size())), 0);
   fMidiOut->sendMessageNow(juce::MidiMessage(message.data(), static_cast<int>(message.size())));
}

void FaustMidi::decodeMessages(const juce::MidiMessage &message)
{
   const juce::uint8* data = message.getRawData();
   const int channel = message.getChannel() - 1;
   const double time = message.getTimeStamp();

   if(message.isNoteOff()) {
      handleKeyOff(time, channel, data[1], data[2]);
   } else if (message.isNoteOn()) {
      handleKeyOn(time, channel, data[1], data[2]);
   } else if (message.isAftertouch()) {
      handlePolyAfterTouch(time, channel, data[1], data[2]);
   } else if (message.isController()) {
      handleCtrlChange(time, channel, data[1], data[2]);
   } else if (message.isProgramChange()) {
      handleProgChange(time, channel, data[1]);
   } else if (message.isChannelPressure()) {
      handleAfterTouch(time, channel, data[1]);
   } else if (message.isPitchWheel()) {
      handlePitchWheel(time, channel, data[1], data[2]);
   } else if (message.isMidiClock()) {
      handleClock(time);
      // We can consider start and continue as identical messages.
   } else if (message.isMidiStart() || message.isMidiContinue()) {
      handleStart(time);
   } else if (message.isMidiStop()) {
      handleStop(time);
   } else if (message.isSysEx()) {
      std::vector<unsigned char> sysex(data, data + message.getRawDataSize());
      handleSysex(time, sysex);
   } else {
      std::cerr << "Unused MIDI message" << std::endl;
   }

}

void FaustMidi::handleIncomingMidiMessage(juce::MidiInput*, const juce::MidiMessage &message)
{
   decodeMessages(message);
}
