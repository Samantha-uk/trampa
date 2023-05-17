/*
--.--
  |  ,---.,---.,-.-.,---.,---.
  |  |    ,---|| | ||   |,---|
  `  `    `---^` ' '|---'`---^
                    |
(C) 2023 Samantha-uk
https://github.com/samantha-uk/trampa
*/

#ifndef _MIDI_H_
#define _MIDI_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "tusb.h"

// MIDI Message
struct MIDIMessageRaw {
  uint8_t status;
  uint8_t arg1;
  uint8_t arg2;
};
// Queue of MIDI messages
// Add MIDI message to queue
// Get MIDI message from queue and xmit it

enum class MIDIMsgType { PROGRAM_CHANGE, CONTROL_CHANGE };

// Single MIDI Message definition
struct MIDIMessage {
  int channel;
  MIDIMsgType msgType;
  int lowValue;
  int highValue;
};

class MIDI {
 public:
  MIDI(int8_t cableNumber = 0);
  ~MIDI();
  // Sends a pitchBend MIDI command to the output
  void pitchBend(uint16_t value, int8_t channel = -1);

  // Sends a noteOn MIDI command to the output
  void noteOn(uint8_t note, uint8_t velocity, int8_t channel = -1);

  // Sends a noteOff MIDI command to the output
  void noteOff(uint8_t note, uint8_t velocity, int8_t channel = -1);

  // Sends a programChange MIDI command to the output
  void programChange(uint8_t program, int8_t channel = -1);

  // Sends a allNotesOff MIDI command to the output
  void allNotesOff(int8_t channel = -1);

  // Sends a resetAllControllers  MIDI command to the output
  void resetAllControllers(int8_t channel = -1);

  // Sends a localControl  MIDI command to the output
  void localControl(bool active, int8_t channel = -1);

  // Sends a control change MIDI command to the output
  void controlChange(uint8_t msg, uint8_t value, int8_t channel = -1);

 private:
  int8_t _cableNumber = 0;
};

#ifdef __cplusplus
}
#endif
#endif /* _MIDI_H_ */