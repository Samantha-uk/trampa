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
#include "midibutton.h"

MIDIButton::MIDIButton(MIDI *MIDIusb, int8_t id, uint8_t pin, const char *name,
                       unsigned portSHORT stackDepth, TaskPriority priority)
    : Thread{stackDepth, priority, name},
      _id(id),
      _pin(pin),
      _MIDIusb(MIDIusb) {
  gpio_init(_pin);
  gpio_set_dir(_pin, GPIO_IN);
  gpio_pull_up(_pin);
}

MIDIButton::~MIDIButton() {}
void MIDIButton::setConfig(MIDIButtonConfig *config) { _config = config; }

void MIDIButton::Main() {
  _button = new Button(_id, _pin);
  _button->setClickDetection(_config->detectClicks);
  _button->setEventHandler(this);

  TickType_t xPreviousWakeTime;
  u_int32_t elapsed = 0;

  // Initialise the xPreviousWakeTime variable with the current time.
  xPreviousWakeTime = xTaskGetTickCount();
  absolute_time_t start;
  do {
    vTaskDelayUntil(&xPreviousWakeTime, pdMS_TO_TICKS(5));
    start = get_absolute_time();
    _button->check();
    elapsed = absolute_time_diff_us(start, get_absolute_time());
    // if (elapsed > 2) printf("Button Scan Time [%u]\r\n", elapsed);
  } while (true);
}

void MIDIButton::handleEvent(int id, ButtonEvent event, int clicks) {
  switch (event) {
    case ButtonEvent::PRESS:
      printf("Button[%u]- Press \r\n", id);
      // if latching then send alternate hi/low values
      break;
    case ButtonEvent::RELEASE:
      printf("Button[%u]- Release \r\n", id);
      // If latching do nothing
      break;
    case ButtonEvent::CLICK:
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
    case ButtonEvent::HOLD:
      switch (clicks) {
        case 0:
          printf("Button[%u]- Hold \r\n", id);
          break;
        case 1:
          printf("Button[%u]- Hold(Shifted) \r\n", id);
          break;
      }
      break;
    case ButtonEvent::REPEAT:
      switch (clicks) {
        case 0:
          printf("Button[%u]- Repeat \r\n", id);
          break;
        case 1:
          printf("Button[%u]- Repeat(Shifted) \r\n", id);
          break;
      }
      break;
    case ButtonEvent::HOLD_RELEASE:
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

  // Send the corresponding message
  /*   MIDIMessage *message = &(messageBank[id - 1][event]);
    switch (message->msgType) {
      case MIDIMsgType::CONTROL_CHANGE:
        _MIDIusb->controlChange(message->lowValue, message->highValue,
                                message->channel);
        break;
      case MIDIMsgType::PROGRAM_CHANGE:
        _MIDIusb->programChange(message->lowValue, message->channel);
        break;
    } */
}