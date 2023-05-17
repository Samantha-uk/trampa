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
                       TaskPriority priority)
    : Thread{stackDepth, priority, name},
      _id(id),
      _pin(pin),
      _detectClicks(detectClicks) {
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
  u_int32_t elapsed = 0;

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
  switch (action) {
    case ButtonAction::PRESS:
      printf("Button[%u]- Press \r\n", id);
      // if latching then send alternate hi/low values
      break;
    case ButtonAction::RELEASE:
      printf("Button[%u]- Release \r\n", id);
      // If latching do nothing
      break;
    case ButtonAction::CLICK:
      switch (clicks) {
        case 0:
          printf("Button[%u]- Click \r\n", id);
          break;
        case 1:
          printf("Button[%u]- DoubleClick \r\n", id);
          break;
        case 2:
          printf("Button[%u]- TripleClick \r\n", id);
          break;
      }
      break;
    case ButtonAction::HOLD:
      switch (clicks) {
        case 0:
          printf("Button[%u]- Hold \r\n", id);
          break;
        case 1:
          printf("Button[%u]- Hold(Shifted) \r\n", id);
          break;
      }
      break;
    case ButtonAction::REPEAT:
      switch (clicks) {
        case 0:
          printf("Button[%u]- Repeat \r\n", id);
          break;
        case 1:
          printf("Button[%u]- Repeat(Shifted) \r\n", id);
          break;
      }
      break;
    case ButtonAction::HOLD_RELEASE:
      switch (clicks) {
        case 0:
          printf("Button[%u]- Hold Release \r\n", id);
          break;
        case 1:
          printf("Button[%u]- Hold(Shifted) Release\r\n", id);
          break;
      }
      break;
  }
}