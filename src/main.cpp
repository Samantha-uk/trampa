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
#include <task.h>

#include <bsp/board.h>
#include <ctype.h>

#include "tusb.h"

#include "eventmanager.h"
#include "taskbutton.h"

#define TASKBUTTON_STACK 2048
#define EVENTMANAGER_STACK 2048

TaskHandle_t tMIDITask, tTUDTask;

//----------- PROTOTYPES -----------
void vTUDTask(void *ptr);
void vMIDITask(void *ptr);
void midi_task(void);

//------------- MAIN -------------
int main(void)

{
  // Initalisation
  board_init();
  tud_init(BOARD_TUD_RHPORT);

  // Create and initalise the EventManager
  EventManager eventManager("EventManager", EVENTMANAGER_STACK, TaskPrio_Mid);
  if (!eventManager.init()) {  // Terminate
  }

  // Create the buttons
  TASKButton button1(1, 13, true, "Button1", TASKBUTTON_STACK, TaskPrio_Mid,
                     &eventManager);
  TASKButton button2(2, 12, false, "Button2", TASKBUTTON_STACK, TaskPrio_Mid,
                     &eventManager);
  TASKButton button3(3, 11, false, "Button3", TASKBUTTON_STACK, TaskPrio_Mid,
                     &eventManager);
  TASKButton button4(4, 10, false, "Button4", TASKBUTTON_STACK, TaskPrio_Mid,
                     &eventManager);

  // Create the TunyUSB task
  xTaskCreate(vTUDTask, "TUD", 2048, NULL, TaskPrio_High, &tTUDTask);
  xTaskCreate(vMIDITask, "MIDI", 2048, NULL, TaskPrio_Mid, &tMIDITask);

  // Start FreeRTOS processing all the tasks
  vTaskStartScheduler();
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

void vApplicationStackOverflowHook(TaskHandle_t Task, char *pcTaskName) {
  panic("stack overflow (not the helpful kind) for %s\n", *pcTaskName);
}
