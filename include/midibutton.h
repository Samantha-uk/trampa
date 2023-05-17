/*
--.--
  |  ,---.,---.,-.-.,---.,---.
  |  |    ,---|| | ||   |,---|
  `  `    `---^` ' '|---'`---^
                    |
(C) 2023 Samantha-uk
https://github.com/samantha-uk/trampa
*/

#ifndef _MIDIBUTTON_H_
#define _MIDIBUTTON_H_

#include "taskcpp.h"
#include "button.h"
#include "midi.h"

// Composite class to bind midi and a button
// Holds a set of configured MIDI messages, for all the possible button events
// Holds any state associated with buttons that are latching or
// increment/decrement
#define MIDIBUTTON_STACK 1024
enum class buttonMode {
  FIXED,      // Always use the lowValue as MIDI message value
  TOGGLE,     // Alternate between lowValue & highValue as MIDI message value
  INCREMENT,  // MIDI message value starts at lowValue and increments by 1 for
              // evey message, wraps around to lowValue when highValue has been
              // sent
  DECREMENT   // MIDI message value starts at highValue and decrements by 1 for
              // evey message, wraps around to highValue when lowValue has been
              // sent
};

struct MIDIButtonConfig {
  bool detectClicks;
  buttonMode mode;
  int channel;
  int lowValue;
  int highValue;
  MIDIMessage message[BUTTON_EVENT_COUNT];
};

class MIDIButton : public Thread<MIDIButton>, IButtonEventHandler {
 public:
  MIDIButton(MIDI *MIDIusb, int8_t id, uint8_t pin, const char *name,
             unsigned portSHORT _stackDepth, TaskPriority priority);
  ~MIDIButton();
  void setConfig(MIDIButtonConfig *config);
  void start(const char *name);
  void handleEvent(int id, ButtonEvent event, int clicks);
  void Main();

 private:
  TaskHandle_t button_taskhandle;

  int _id;        // button ID used in callback to identofy button instance
  uint _pin = 0;  // GPIO pin for the button
  MIDI *_MIDIusb = NULL;
  int _previousValue = -1;
  Button *_button = NULL;
  MIDIButtonConfig *_config = NULL;
};

#endif /* _MIDIBUTTON_H_ */