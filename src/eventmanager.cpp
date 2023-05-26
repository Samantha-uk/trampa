/*
--.--
  |  ,---.,---.,-.-.,---.,---.
  |  |    ,---|| | ||   |,---|
  `  `    `---^` ' '|---'`---^
                    |
(C) 2023 Samantha-uk
https://github.com/samantha-uk/trampa
*/

#include <FreeRTOS.h>
#include <queue.h>
#include <stdio.h>
#include <stdlib.h>

#include "eventmanager.h"

EventManager::EventManager(const char *name, unsigned portSHORT stackDepth,
                           TaskPriority priority)
    : Thread{stackDepth, priority, name} {}

EventManager::~EventManager() {}

bool EventManager::init() {
  // Initalisation code
  _eventQueue = xQueueCreate(EVENT_QUEUE_SIZE, sizeof(event));
  return _eventQueue != NULL;
}

void EventManager::Main() {
  event evt;
  do {
    // Read from the event queue, block forever if nothing in queue
    if (xQueueReceive(_eventQueue, &evt, portMAX_DELAY)) {
      switch (evt.eType) {
        case EventType::BUTTON:
          handleButtonEvent(evt);
          break;

        case EventType::EXPRESSION:
          break;

        case EventType::MIDI:
          break;
      }
    }

  } while (true);
}

void EventManager::addEvent(int id, ButtonAction action, int clicks) {
  event evt;
  // Populate the button event
  evt.eType = EventType::BUTTON;
  evt.eData.eButton.id = id;
  evt.eData.eButton.action = action;
  evt.eData.eButton.clicks = clicks;

  // Add button event to the queue
  xQueueSend(_eventQueue, (void *)&evt, (TickType_t)0);
}

void EventManager::handleButtonEvent(event evt) {
  switch (evt.eData.eButton.action) {
    case ButtonAction::PRESS:
      printf("Button[%u]- Press \r\n", evt.eData.eButton.id);
      break;
    case ButtonAction::RELEASE:
      printf("Button[%u]- Release \r\n", evt.eData.eButton.id);
      break;
    case ButtonAction::CLICK:
      switch (evt.eData.eButton.clicks) {
        case 0:
          printf("Button[%u]- Click \r\n", evt.eData.eButton.id);
          break;
        case 1:
          printf("Button[%u]- DoubleClick \r\n", evt.eData.eButton.id);
          break;
        case 2:
          printf("Button[%u]- TripleClick \r\n", evt.eData.eButton.id);
          break;
      }
      break;
    case ButtonAction::HOLD:
      switch (evt.eData.eButton.clicks) {
        case 0:
          printf("Button[%u]- Hold \r\n", evt.eData.eButton.id);
          break;
        case 1:
          printf("Button[%u]- Hold(Shifted) \r\n", evt.eData.eButton.id);
          break;
      }
      break;
    case ButtonAction::REPEAT:
      switch (evt.eData.eButton.clicks) {
        case 0:
          printf("Button[%u]- Repeat \r\n", evt.eData.eButton.id);
          break;
        case 1:
          printf("Button[%u]- Repeat(Shifted) \r\n", evt.eData.eButton.id);
          break;
      }
      break;
    case ButtonAction::HOLD_RELEASE:
      switch (evt.eData.eButton.clicks) {
        case 0:
          printf("Button[%u]- Hold Release \r\n", evt.eData.eButton.id);
          break;
        case 1:
          printf("Button[%u]- Hold(Shifted) Release\r\n", evt.eData.eButton.id);
          break;
      }
      break;
  }
}

void EventManager::handleExpressionEvent(event evt) {}
void EventManager::handleMIDIEvent(event evt) {}