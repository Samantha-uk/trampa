/*
--.--
  |  ,---.,---.,-.-.,---.,---.
  |  |    ,---|| | ||   |,---|
  `  `    `---^` ' '|---'`---^
                    |
(C) 2023 Samantha-uk
https://github.com/samantha-uk/trampa

Adapted from debounce.c https://www.kennethkuhn.com/electronics/debounce.c
/******************************************************************************
debounce.c
written by Kenneth A. Kuhn
version 1.00

This is an algorithm that debounces or removes random or spurious
transistions of a digital signal read as an input by a computer.  This is
particularly applicable when the input is from a mechanical contact.  An
integrator is used to perform a time hysterisis so that the signal must
persistantly be in a logical state (0 or 1) in order for the output to change
to that state.  Random transitions of the input will not affect the output
except in the rare case where statistical clustering is longer than the
specified integration time.

The following example illustrates how this algorithm works.  The sequence
labeled, real signal, represents the real intended signal with no noise.  The
sequence labeled, corrupted, has significant random transitions added to the
real signal.  The sequence labled, integrator, represents the algorithm
integrator which is constrained to be between 0 and 3.  The sequence labeled,
output, only makes a transition when the integrator reaches either 0 or 3.
Note that the output signal lags the input signal by the integration time but
is free of spurious transitions.

real signal 0000111111110000000111111100000000011111111110000000000111111100000
corrupted   0100111011011001000011011010001001011100101111000100010111011100010
integrator  0100123233233212100012123232101001012321212333210100010123233321010
output      0000001111111111100000001111100000000111111111110000000001111111000

I have been using this algorithm for years and I show it here as a code
fragment in C.  The algorithm has been around for many years but does not seem
to be widely known.  Once in a rare while it is published in a tech note.  It
is notable that the algorithm uses integration as opposed to edge logic
(differentiation).  It is the integration that makes this algorithm so robust
in the presence of noise.
******************************************************************************/
#include <stdio.h>
#include "button.h"

Button::Button(int id, uint pin) {
  _id = id;
  _pin = pin;
}

void Button::init(void) {
  gpio_init(_pin);
  gpio_set_dir(_pin, GPIO_IN);
  gpio_pull_up(_pin);
}

void Button::setClickDetection(bool clickDetect) { _clickDetect = clickDetect; }
void Button::setEventHandler(IButtonEventHandler *eventHandler) {
  _eventHandler = eventHandler;
}

bool Button::pressed() {
  /*
  Step 1: Update the integrator based on the input signal.  Note that the
  integrator follows the input, decreasing or increasing towards the limits as
  determined by the input state (0 or 1).
  */
  if (gpio_get(_pin) == 1) {
    if (_integrator > 0) _integrator--;
  } else if (_integrator < INTEGRATOR_MAX)
    _integrator++;

  /*
  Step 2: Update the output state based on the integrator.  Note that the
  output will only change states if the integrator has reached a limit, either
  0 or MAXIMUM.
   */
  if (_integrator == 0)
    _pressed = false;
  else if (_integrator >= INTEGRATOR_MAX) {
    _integrator =
        INTEGRATOR_MAX; /* defensive code if integrator got corrupted */
    _pressed = true;
  }
  return _pressed;
}
void Button::setState(ButtonState state) {
  printf("State button%d [%d]->[%d]\r\n", _id, _state, state);
  _state = state;
}

void Button::check(void) {
  // Get the debounced current debounced pressed state of the button
  bool isPressed = pressed();

  // Process the state machine for the button
  switch (_state) {
    case ButtonState::IDLE:
      if (isPressed) {
        // Capture the time
        _buttonPressedTime = get_absolute_time();

        // Reset _clickCount
        _clickCount = 0;

        // should we send a PRESS event
        if (!_clickDetect) fireEvent(ButtonEvent::PRESS);

        // Change to PRESSED state
        setState(ButtonState::PRESSED);
      }
      break;

    case ButtonState::PRESSED:
      if (!isPressed) {      // The button is now released
        if (_clickDetect) {  // We ARE detecting clicks
          // Capture the time
          _buttonReleasedTime = get_absolute_time();

          // Change to RELEASED state
          setState(ButtonState::RELEASED);
        } else {  // We ARE NOT detecting clicks
          fireEvent(ButtonEvent::RELEASE);
          // Reset _clickCount
          _clickCount = 0;

          // Change to IDLE state
          setState(ButtonState::IDLE);
        }
      } else {  // The button is still pressed
        // If we are detecting clicks and the time since _buttonDownTime is
        // longer than _holdDelay
        if (_clickDetect &&
            absolute_time_diff_us(_buttonPressedTime, get_absolute_time()) >=
                _holdDelay) {
          fireEvent(ButtonEvent::HOLD);

          // Capture the time
          _buttonRepeatTime = get_absolute_time();

          // Change to HOLD state
          setState(ButtonState::HOLD);
        }
      }
      break;

    case ButtonState::RELEASED:
      if (isPressed) {  // The button is pressed
        // Capture the time
        _buttonPressedTime = get_absolute_time();

        // Increment _clickCount
        _clickCount++;

        // Change to PRESSED state
        setState(ButtonState::PRESSED);
      } else {  // The button is still releasaed
        // check to see if _clickDelay has passed since we entered RELEASED
        // state
        if (absolute_time_diff_us(_buttonReleasedTime, get_absolute_time()) >=
            _clickDelay) {
          // _clickDelay time has passed so we report the number of clicks
          fireEvent(ButtonEvent::CLICK);

          // Reset _clickCount
          _clickCount = 0;

          // Change to IDLE state
          setState(ButtonState::IDLE);
        }
      }
      break;

    case ButtonState::HOLD:
      // If the button is now released
      // _clickCount is used to indicate if any single clicks preceeded the hold
      // These are used as "shifts" to potentially alter the behaviour of
      // hold/repeat events.
      if (!isPressed) {  // The button is now released
        fireEvent(ButtonEvent::HOLD_RELEASE);

        // Reset _clickCount
        _clickCount = 0;

        // Change to IDLE state
        setState(ButtonState::IDLE);
      } else {  // The button is still pressed
        // If time down is longer than _repeatDelay
        if (absolute_time_diff_us(_buttonRepeatTime, get_absolute_time()) >=
            _repeatDelay) {
          fireEvent(ButtonEvent::REPEAT);

          // Reset _buttonHoldTime
          _buttonRepeatTime = get_absolute_time();
        }
      }
      break;
  }
}

void Button::fireEvent(ButtonEvent event) {
  // Check to make sure that we have an eventHandler
  if (_eventHandler) {
    _eventHandler->handleEvent(_id, event, _clickCount);
  }
}