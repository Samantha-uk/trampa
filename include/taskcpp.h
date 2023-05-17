
/*
--.--
  |  ,---.,---.,-.-.,---.,---.
  |  |    ,---|| | ||   |,---|
  `  `    `---^` ' '|---'`---^
                    |
(C) 2023 Samantha-uk
https://github.com/samantha-uk/trampa

Derived from
*/

#ifndef _TASKCPP_H_
#define _TASKCPP_H_

#include "FreeRTOS.h"
#include "task.h"

enum TaskPriority {
  TaskPrio_Idle = 0,  ///< Non-Real Time operations. tasks that don't block
  TaskPrio_Low = ((configMAX_PRIORITIES) > 1),  ///< Non-Critical operations
  TaskPrio_HMI = (TaskPrio_Low + ((configMAX_PRIORITIES) >
                                  5)),  ///< Normal User Interface Level
  TaskPrio_Mid =
      ((configMAX_PRIORITIES) /
       2),  ///< Semi-Critical, have deadlines, not a lot of processing
  TaskPrio_High =
      ((configMAX_PRIORITIES)-1 -
       ((configMAX_PRIORITIES) >
        4)),  ///< Urgent tasks, short deadlines, not much processing
  TaskPrio_Highest =
      ((configMAX_PRIORITIES)-1)  ///< Critical Tasks, Do NOW, must be quick
                                  ///< (Used by FreeRTOS)
};

template <typename T>
class Thread {
 public:
  Thread(unsigned portSHORT _stackDepth, UBaseType_t _priority,
         const char* _name = "") {
    xTaskCreate(task, _name, _stackDepth, this, _priority, &this->taskHandle);
  }

  TaskHandle_t GetHandle() { return this->taskHandle; }

  void Main() { static_cast<T&>(*this).Main(); }

 private:
  static void task(void* _params) {
    Thread* p = static_cast<Thread*>(_params);
    p->Main();
  }

  TaskHandle_t taskHandle;
};

#endif /* _TASKCPP_H_ */