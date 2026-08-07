#include "rtc.h"
#include <Arduino.h>

RTC_DS3231 rtc;
volatile bool rtcAlarmFired = false;

static void rtcISR() {
  rtcAlarmFired = true;
}

void setupRTC() {
  rtc.begin();

  rtc.clearAlarm(1);
  rtc.clearAlarm(2);
  pinMode(RTC_INT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RTC_INT_PIN), rtcISR, FALLING);
}

void setNextAlarm(const ReminderTime* reminders, int count) {
  DateTime now = rtc.now();
  int currentMins = now.hour() * 60 + now.minute();

  int bestMins = -1;
  int bestIdx = -1;
  for (int i = 0; i < count; i++) {
    int mins = reminders[i].hour * 60 + reminders[i].minute;
    if (mins > currentMins && (bestIdx == -1 || mins < bestMins)) {
      bestMins = mins;
      bestIdx = i;
    }
  }

  DateTime alarmTime;
  if (bestIdx != -1) {
    alarmTime = DateTime(now.year(), now.month(), now.day(),
                         reminders[bestIdx].hour, reminders[bestIdx].minute, 0);
  } else {
    DateTime tomorrow = now + TimeSpan(1, 0, 0, 0);
    alarmTime = DateTime(tomorrow.year(), tomorrow.month(), tomorrow.day(), 0, 0, 0);
  }

  rtc.clearAlarm(1);
  rtc.setAlarm1(alarmTime, DS3231_A1_Hour);
}
