/*
The MIT License (MIT)

Copyright (c) 2019 Ha Thach (tinyusb.org)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
 */

#include "FreeRTOS.h"
#include "taskcpp.h"
#include "tusb.h"
#include <bsp/board.h>
#include <ctype.h>
#include <FreeRTOS.h>
#include <pico/stdio.h>
#include <pico/stdlib.h>
#include <queue.h>
#include <stdarg.h>
#include <string.h>
#include <task.h>
#include "midibutton.h"

TaskHandle_t tMIDITask, tTUDTask;

/*----------- PROTOTYPES -----------*/
void vTUDTask(void *ptr);
void vMIDITask(void *ptr);
void midi_task(void);
int write_serial(const char *, ...) __attribute__((format(printf, 1, 0)));

/*------------- MAIN -------------*/
int main(void)

{
  board_init();
  tud_init(BOARD_TUD_RHPORT);

  MIDI MIDIusb(0);

  // Create the buttons
  MIDIButton button1(&MIDIusb, 1, 13, "Button1", MIDIBUTTON_STACK,
                     TaskPrio_Mid);
  MIDIButtonConfig config1 = {
      true,                // DetectClicks
      buttonMode::TOGGLE,  // button mode
      1,                   // channel
      0,                   // Low value
      127,                 // High value
  };
  button1.setConfig(&config1);

  xTaskCreate(vTUDTask, "TUD", 2048, NULL, TaskPrio_High, &tTUDTask);
  xTaskCreate(vMIDITask, "MIDI", 2048, NULL, TaskPrio_Mid, &tMIDITask);
  printf("Starting scheduler.\r\n");
  vTaskStartScheduler();

  vTUDTask(NULL);
  return 0;
}

void vTUDTask(void *ptr) {
  TickType_t xPreviousWakeTime;
  // Initialise the xPreviousWakeTime variable with the current time.
  xPreviousWakeTime = xTaskGetTickCount();

  do {
    xTaskDelayUntil(&xPreviousWakeTime, pdMS_TO_TICKS(2));
    tud_task();
  } while (true);
}

void vMIDITask(void *ptr) {
  TickType_t xPreviousWakeTime;
  // Initialise the xPreviousWakeTime variable with the current time.
  xPreviousWakeTime = xTaskGetTickCount();

  do {
    xTaskDelayUntil(&xPreviousWakeTime, pdMS_TO_TICKS(1000));
    midi_task();
    printf(".");

  } while (true);
}

///--------------------------------------------------------------------+
// MIDI Task
//--------------------------------------------------------------------+

// Variable that holds the current position in the sequence.
uint32_t note_pos = 0;

// Store example melody as an array of note values
static const uint8_t note_sequence[] = {
    74, 78, 81, 86,  90, 93, 98, 102, 57, 61,  66, 69, 73, 78, 81, 85,
    88, 92, 97, 100, 97, 92, 88, 85,  81, 78,  74, 69, 66, 62, 57, 62,
    66, 69, 74, 78,  81, 86, 90, 93,  97, 102, 97, 93, 90, 85, 81, 78,
    73, 68, 64, 61,  56, 61, 64, 68,  74, 78,  81, 86, 90, 93, 98, 102};

void midi_task(void) {
  uint8_t const cable_num = 0;  // MIDI jack associated with USB endpoint
  uint8_t const channel = 0;    // 0 for channel 1

  // The MIDI interface always creates input and output port/jack descriptors
  // regardless of these being used or not. Therefore incoming traffic should
  // be read (possibly just discarded) to avoid the sender blocking in IO
  uint8_t packet[4];
  while (tud_midi_available()) tud_midi_packet_read(packet);

  // Previous positions in the note sequence.
  int previous = (int)(note_pos - 1);

  // If we currently are at position 0, set the
  // previous position to the last note in the sequence.
  if (previous < 0) previous = sizeof(note_sequence) - 1;

  // Send Note On for current position at full velocity (127) on channel 1.
  uint8_t note_on[3] = {0x90 | channel, note_sequence[note_pos], 127};
  tud_midi_stream_write(cable_num, note_on, 3);

  // Send Note Off for previous note.
  uint8_t note_off[3] = {0x80 | channel, note_sequence[previous], 0};
  tud_midi_stream_write(cable_num, note_off, 3);

  // Increment position
  note_pos++;

  // If we are at the end of the sequence, start over.
  if (note_pos >= sizeof(note_sequence)) note_pos = 0;
}

int write_serial(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  char message[256];
  int ret_val = vsprintf(message, fmt, args);
  va_end(args);
  tud_cdc_write_str(message);
  tud_cdc_write_flush();
  return ret_val;
}

void vApplicationStackOverflowHook(TaskHandle_t Task, char *pcTaskName) {
  panic("stack overflow (not the helpful kind) for %s\n", *pcTaskName);
}
