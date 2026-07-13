#pragma once
#include <RTClib.h>
#include "tasks.h"

#define RTC_INT_PIN 2  // INT0 — DS3231 INT/SQW pin

extern RTC_DS3231 rtc;
extern volatile bool rtcAlarmFired;

void setupRTC();
void setNextAlarm(const ReminderTime* reminders, int count);
