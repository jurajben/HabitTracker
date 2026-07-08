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
};

// Read task name from PROGMEM into a caller-supplied buffer
inline void taskGetName(const Task* tasks, int i, char* buf) {
  memcpy_P(buf, tasks[i].name, TASK_NAME_LEN);
}
