/*
--.--
  |  ,---.,---.,-.-.,---.,---.
  |  |    ,---|| | ||   |,---|
  `  `    `---^` ' '|---'`---^
                    |
(C) 2023 Samantha-uk
https://github.com/samantha-uk/trampa

Inspired by https://github.com/pschatzmann/arduino-midi
*/
#include "midi.h"

MIDI::MIDI(int8_t cableNumber) { _cableNumber = cableNumber; }
MIDI::~MIDI() {}

// Sends a pitchBend MIDI command to the output
void MIDI::pitchBend(uint16_t value, int8_t channel) {}

// Sends a noteOn MIDI command to the output
void MIDI::noteOn(uint8_t note, uint8_t velocity, int8_t channel) {
  MIDIMessageRaw message;
  message.status = 0b1001 << 4 | channel;
  message.arg1 = note;
  message.arg2 = velocity;
  tud_midi_stream_write(_cableNumber, (uint8_t*)&message, sizeof(message));
}

// Sends a noteOff MIDI command to the output
void MIDI::noteOff(uint8_t note, uint8_t velocity, int8_t channel) {
  MIDIMessageRaw message;
  message.status = 0b1000 << 4 | channel;
  message.arg1 = note;
  message.arg2 = velocity;
  tud_midi_stream_write(_cableNumber, (uint8_t*)&message, sizeof(message));
}

// Sends a programChange MIDI command to the output
void MIDI::programChange(uint8_t program, int8_t channel) {
  MIDIMessageRaw message;
  uint8_t value = program & 0b1111111;
  message.status = 0b1100 << 4 | channel;
  message.arg1 = value;
  tud_midi_stream_write(_cableNumber, (uint8_t*)&message, sizeof(message));
}

// Sends a allNotesOff MIDI command to the output
void MIDI::allNotesOff(int8_t channel) {}

// Sends a resetAllControllers  MIDI command to the output
void MIDI::resetAllControllers(int8_t channel) {}

// Sends a localControl  MIDI command to the output
void MIDI::localControl(bool active, int8_t channel) {}

// Sends a control change MIDI command to the output
void MIDI::controlChange(uint8_t msg, uint8_t value, int8_t channel) {
  MIDIMessageRaw message;
  message.status = 0b1011 << 4 | channel;
  message.arg1 = msg;
  message.arg2 = value;
  tud_midi_stream_write(_cableNumber, (uint8_t*)&message, sizeof(message));
}
