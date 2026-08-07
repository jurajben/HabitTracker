#pragma once
#include <stdint.h>
#include <avr/pgmspace.h>

#define TASK_NAME_LEN  16
#define TASK_DESC_LEN  48

struct ReminderTime {
  uint8_t hour;
  uint8_t minute;
};

struct Task {
  char name[TASK_NAME_LEN];
  char description[TASK_DESC_LEN];
  bool required;
  ReminderTime reminder;
  bool completed;
};

inline void taskGetName(const Task* tasks, int i, char* buf) {
  memcpy_P(buf, tasks[i].name, TASK_NAME_LEN);
}

inline void taskGet(const Task* tasks, int i, Task* out) {
  memcpy_P(out, &tasks[i], sizeof(Task));
}

inline void taskGetReminder(const Task* tasks, int i, ReminderTime* out) {
  memcpy_P(out, &tasks[i].reminder, sizeof(ReminderTime));
}

extern bool taskCompleted[];

inline bool taskGetCompleted(const Task* /*tasks*/, int i) {
  return taskCompleted[i];
}

inline void buildReminderTimes(const Task* tasks, int taskCount, ReminderTime* out) {
  for (int i = 0; i < taskCount; i++)
    taskGetReminder(tasks, i, &out[i]);
}
