/*
--.--
  |  ,---.,---.,-.-.,---.,---.
  |  |    ,---|| | ||   |,---|
  `  `    `---^` ' '|---'`---^
                    |
(C) 2023 Samantha-uk
https://github.com/samantha-uk/trampa
*/

#ifndef _EVENTS_H_
#define _EVENTS_H_

#include "pico/stdlib.h"
#include <hardware/gpio.h>
#include "FreeRTOS.h"
#include <queue.h>
#include "taskcpp.h"
#include "taskbutton.h"

// Events
// Button Event
// ExpressionPedal Event
// MIDI Event
union event {
  ButtonEvent eButton;
};

enum class TrampaEventType { BUTTON, EXPRESSION, MIDI };

struct TrampaEvent {
  TrampaEventType type;
  event evt;
};

class TaskEvent : public Thread<TaskEvent> {
 public:
  TaskEvent();
  ~TaskEvent();
  void Main();
};

#endif /* _EVENTS_H_ */