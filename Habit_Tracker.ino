#include "display.h"
#include "input.h"
#include "tasks.h"

const Task tasks[] PROGMEM = {
    {"Drink water", "Drink at least 2L of water in a day", true, {18, 0}, false},
    {"Morning run", "Run at least 3km", false, {6, 30}, true},
    {"Read 30 min", "Read a book for at least 30 minutes", false, {21, 0}, true},
    {"Sleep by 23", "Be in bed with lights off before 23:00", true, {22, 30}, false},
    {"Stretch", "Full body stretching routine, 10 minutes", false, {8, 0}, false},
    {"Journal", "Write at least 3 sentences about your day", false, {22, 0}, false},
    {"Work on project", "Spend 1 hour on personal projects", true, {17, 0}, true}};
const int taskCount = sizeof(tasks) / sizeof(tasks[0]);

Screen currentScreen = SCREEN_TITLE;
int selectedIndex = 0;
int scrollOffset = 0;
bool detailOpen = false;

#define VISIBLE_ROWS 5

void handleInput(uint8_t pin) {
  bool changed = true;

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
  setupDisplay();
  drawCurrentScreen(currentScreen, tasks, taskCount, selectedIndex,
                    scrollOffset);
}

void loop() {
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
