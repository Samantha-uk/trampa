/*
--.--
  |  ,---.,---.,-.-.,---.,---.
  |  |    ,---|| | ||   |,---|
  `  `    `---^` ' '|---'`---^
                    |
(C) 2023 Samantha-uk
https://github.com/samantha-uk/trampa
*/

#ifndef _TASKBUTTON_H_
#define _TASKBUTTON_H_

#include "taskcpp.h"
#include "button.h"
#include "midi.h"

// Composite class to bind midi and a button
// Holds a set of configured MIDI messages, for all the possible button events
// Holds any state associated with buttons that are latching or
// increment/decrement
#define TASKBUTTON_STACK 1024
// TODO Move this enum to the eventProcessor
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

class TASKButton : public Thread<TASKButton>, IButtonActionHandler {
 public:
  TASKButton(int8_t id, uint8_t pin, bool detectClicks, const char *name,
             unsigned portSHORT _stackDepth, TaskPriority priority);
  ~TASKButton();
  void handleAction(int id, ButtonAction action, int clicks);
  void Main();

 private:
  TaskHandle_t button_taskhandle;

  int _id;        // button ID used in callback to identofy button instance
  uint _pin = 0;  // GPIO pin for the button
  bool _detectClicks = false;
  Button *_button = NULL;
};

#endif /* _TASKBUTTON_H_ */