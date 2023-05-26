/*
--.--
  |  ,---.,---.,-.-.,---.,---.
  |  |    ,---|| | ||   |,---|
  `  `    `---^` ' '|---'`---^
                    |
(C) 2023 Samantha-uk
https://github.com/samantha-uk/trampa
*/

#ifndef _EVENTMANAGER_H_
#define _EVENTMANAGER_H_

#include <queue.h>
#include "taskbutton.h"
#include "taskcpp.h"

#define EVENT_QUEUE_SIZE 30

// Events
// Button Event
// ExpressionPedal Event
// MIDI Event

enum class EventType { BUTTON, EXPRESSION, MIDI };

union eventData {
  ButtonEvent eButton;
};

struct event {
  EventType eType;
  eventData eData;
};

class EventManager : public Thread<EventManager>, public IButtonEventManager {
 public:
  EventManager(const char *name, unsigned portSHORT stackDepth,
               TaskPriority priority);
  ~EventManager();
  bool init(void);
  void Main();

  // Overloaded
  void addEvent(int id, ButtonAction action, int clicks);

  void handleButtonEvent(event evt);
  void handleExpressionEvent(event evt);
  void handleMIDIEvent(event evt);

 private:
  QueueHandle_t _eventQueue = NULL;
};

#endif /* _EVENTMANAGER_H_ */