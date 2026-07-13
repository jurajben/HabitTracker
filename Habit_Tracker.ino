#include "buzzer.h"
#include "display.h"
#include "input.h"
#include "rtc.h"
#include "tasks.h"

// Compile-time helpers — extract H:M from __TIME__ ("HH:MM:SS") and add offset
#define CT_H ((__TIME__[0]-'0')*10 + (__TIME__[1]-'0'))
#define CT_M ((__TIME__[3]-'0')*10 + (__TIME__[4]-'0'))
#define REM_H(off) ((uint8_t)(((CT_H) + ((CT_M)+(off))/60) % 24))
#define REM_M(off) ((uint8_t)(((CT_M)+(off)) % 60))

const Task tasks[] PROGMEM = {
    {"Drink water", "Drink at least 2L of water in a day", true, {REM_H(3), REM_M(3)}, false},
    {"Morning run", "Run at least 3km", false, {REM_H(1), REM_M(1)}, true},
    {"Read 30 min", "Read a book for at least 30 minutes", false, {21, 0}, true},
    {"Sleep by 23", "Be in bed with lights off before 23:00", true, {REM_H(3), REM_M(3)}, false},
    {"Stretch", "Full body stretching routine, 10 minutes", false, {8, 0}, true},
    {"Journal", "Write at least 3 sentences about your day", false, {22, 0}, false},
    {"Work on project", "Spend 1 hour on personal projects", true, {17, 0}, true}};
const int taskCount = sizeof(tasks) / sizeof(tasks[0]);

Screen currentScreen = SCREEN_TITLE;
int selectedIndex = 0;
int scrollOffset = 0;
bool detailOpen = false;
bool reminderTriggered = false;


// Debug set of reminders
ReminderTime reminders[taskCount] = {
  {REM_H(1), REM_M(1)},
  {REM_H(12), REM_M(12)},
  {REM_H(9), REM_M(9)},
  {REM_H(4), REM_M(4)},
  {REM_H(5), REM_M(5)},
  {REM_H(3), REM_M(3)},
  {REM_H(2), REM_M(2)},
};

#define VISIBLE_ROWS 5

void handleInput(uint8_t pin) {
  bool changed = true;

  if (reminderTriggered) {
    if (pin) {
      reminderTriggered = false;
      drawCurrentScreen(currentScreen, tasks, taskCount, selectedIndex,
                        scrollOffset);
      if (detailOpen) drawDetailOverlay(tasks, selectedIndex);
    }
    return;
  }

  switch (currentScreen) {
  case SCREEN_TITLE:
    if (pin == PIN_RHT)
      currentScreen = SCREEN_LIST;
    else
      changed = false;
    break;

  case SCREEN_LIST:
    if (detailOpen) {
      if (pin == PIN_MID) detailOpen = false;
      else changed = false;
      break;
    }
    if (pin == PIN_MID) {
      detailOpen = true;
    } else if (pin == PIN_RHT) {
      currentScreen = SCREEN_GRAPH;
    } else if (pin == PIN_LET) {
      currentScreen = SCREEN_TITLE;
    } else if (pin == PIN_UP && selectedIndex > 0) {
      selectedIndex--;
      if (selectedIndex < scrollOffset)
        scrollOffset = selectedIndex;
    } else if (pin == PIN_DWN && selectedIndex < taskCount - 1) {
      selectedIndex++;
      if (selectedIndex >= scrollOffset + VISIBLE_ROWS)
        scrollOffset = selectedIndex - VISIBLE_ROWS + 1;
    } else {
      changed = false;
    }
    break;

  case SCREEN_GRAPH:
    if (pin == PIN_LET)
      currentScreen = SCREEN_LIST;
    else if (pin == PIN_RHT)
      currentScreen = SCREEN_SETTINGS;
    else
      changed = false;
    break;

  case SCREEN_SETTINGS:
    if (pin == PIN_LET)
      currentScreen = SCREEN_GRAPH;
    else
      changed = false;
    break;
  }

  if (changed) {
    drawCurrentScreen(currentScreen, tasks, taskCount, selectedIndex,
                      scrollOffset);
    if (detailOpen) drawDetailOverlay(tasks, selectedIndex);
  }
}

void setup() {
  setupInput();
  setupBuzzer();
  setupDisplay();

  setupRTC();
  rtc.adjust(DateTime(__DATE__, __TIME__));
  //buildReminderTimes(tasks, taskCount, reminders);
  setNextAlarm(reminders, taskCount);

  // Debug: show RTC time and scheduled alarm for 4 seconds
  DateTime now = rtc.now();
  DateTime alarm = rtc.getAlarm1();
  char buf1[17], buf2[17];
  sprintf(buf1, "Now %02d:%02d:%02d", now.hour(), now.minute(), now.second());
  sprintf(buf2, "Alm %02d:%02d:%02d", alarm.hour(), alarm.minute(), alarm.second());
  drawMessage(buf1, buf2);
  delay(4000);

  drawCurrentScreen(currentScreen, tasks, taskCount, selectedIndex,
                    scrollOffset);
}

void loop() {
  if (rtcAlarmFired) {
    rtcAlarmFired = false;
    DateTime now = rtc.now();
    if (now.hour() == 0 && now.minute() == 0) {
      // TODO: day reset
      setNextAlarm(reminders, taskCount);
    } else {
      reminderTriggered = true;
      buzzReminder();
      drawReminderScreen(tasks, taskCount, now.hour(), now.minute());
      setNextAlarm(reminders, taskCount);
    }
  }

  if (pressedPin != 0) {
    static unsigned long lastHandled = 0;
    unsigned long now = millis();
    if (now - lastHandled > DEBOUNCE_MS) {
      lastHandled = now;
      handleInput(pressedPin);
    }
    pressedPin = 0;
  }
}
