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
#include "taskbutton.h"

TASKButton::TASKButton(int8_t id, uint8_t pin, bool detectClicks,
                       const char *name, unsigned portSHORT stackDepth,
                       TaskPriority priority, IButtonEventManager *eventManager)
    : Thread{stackDepth, priority, name},
      _id(id),
      _pin(pin),
      _detectClicks(detectClicks),
      _eventManager(eventManager) {
  gpio_init(_pin);
  gpio_set_dir(_pin, GPIO_IN);
  gpio_pull_up(_pin);
}

TASKButton::~TASKButton() { delete _button; }

void TASKButton::Main() {
  _button = new Button(_id, _pin);
  _button->setClickDetection(_detectClicks);
  _button->setEventHandler(this);

  TickType_t xPreviousWakeTime;
  int64_t elapsed = 0;

  // Initialise the xPreviousWakeTime variable with the current time.
  xPreviousWakeTime = xTaskGetTickCount();
  absolute_time_t start;
  do {
    xTaskDelayUntil(&xPreviousWakeTime, pdMS_TO_TICKS(5));
    start = get_absolute_time();
    _button->check();
    elapsed = absolute_time_diff_us(start, get_absolute_time());
  } while (true);
}

void TASKButton::handleAction(int id, ButtonAction action, int clicks) {
  // Submit a button event to the EventManager
  _eventManager->addEvent(id, action, clicks);
}