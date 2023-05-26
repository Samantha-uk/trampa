/*
--.--
  |  ,---.,---.,-.-.,---.,---.
  |  |    ,---|| | ||   |,---|
  `  `    `---^` ' '|---'`---^
                    |
(C) 2023 Samantha-uk
https://github.com/samantha-uk/trampa

Inspired by
https://github.com/bxparks/AceButton


Button supports the following actions:
press - the button is pressed
release - the button is released
tap - the button is tapped
longtap - the button is long tapped
doubletap - the button is double tapped
longdoubletap - the button is tapped then long tapped
hold - The button is held down
repeat - The button is held down and sends repeats
*/

#ifndef _BUTTON_H_
#define _BUTTON_H_

#include "pico/stdlib.h"
#include <hardware/gpio.h>
#include <ctype.h>

#define INTEGRATOR_MAX 10

enum class ButtonState { IDLE, PRESSED, RELEASED, HOLD };
enum class ButtonAction {
  PRESS,
  RELEASE,
  CLICK,
  HOLD,
  REPEAT,
  HOLD_RELEASE,
  BUTTON_EVENT_COUNT
};

struct ButtonEvent {
  int id;
  ButtonAction action;
  int clicks;
};

class IButtonActionHandler {
 public:
  virtual void handleAction(int id, ButtonAction action, int clicks) = 0;
};

class Button {
 public:
  Button(int id, uint pin);
  ~Button(){};

  // Setup the GPIO pin
  void init(void);

  // Perform a debounced gpio_get
  bool pressed(void);

  // advance the button through its FSM, firing callbacks as appropriate
  void check(void);

  // Set the mode of the button
  void setClickDetection(bool clickDetect);
  void setEventHandler(IButtonActionHandler *eventHandler);

 private:
  void fireAction(ButtonAction action);
  void setState(ButtonState state);
  // All times in microseconds
  int64_t _clickDelay = 180000;  // Used to check if we should stop checking for
                                 // more clicks and exit the RELEASED state
  int64_t _holdDelay =
      600000;  // Used to determine if a button HOLD has occured
  int64_t _repeatDelay =
      500000;  // USed to determine if it is time to send a REPEAT event
  bool _clickDetect =
      false;  // Should we check for clicks/hold or simply press/release
  IButtonActionHandler *_eventHandler = NULL;

  int _id;        // button ID used in callback to identify button instance
  uint _pin = 0;  // GPIO pin for the button
  uint _integrator = 0;   // Integrator value for the button, used in debouncing
  bool _pressed = false;  // Pressed state of the button based on integration of
  // GPIO input, used only within the pressed() method
  int8_t _clickCount = 0;  // Used to tally the number of singleClicks detected
  ButtonState _state = ButtonState::IDLE;  // Initial state of the FSM
  absolute_time_t
      _buttonPressedTime;  // Used to check if we need to enter HOLD state
  absolute_time_t
      _buttonReleasedTime;  // Used to determine if we should finish checking
                            // for more clicks and send the click event
  absolute_time_t
      _buttonRepeatTime;  // Used to determine if we should send a REPEAT event
};

#endif /* _BUTTON_H_ */