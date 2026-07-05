#include "display.h"
#include "input.h"

const char* tasks[] = {
  "Drink water",
  "Morning run",
  "Read 30 min",
  "Meditate",
  "No sugar",
  "Sleep by 23:00",
  "Stretch",
  "Journal",
};
const int taskCount = sizeof(tasks) / sizeof(tasks[0]);

Screen currentScreen = SCREEN_TITLE;
int selectedIndex    = 0;
int scrollOffset     = 0;

#define VISIBLE_ROWS 5

void handleInput(uint8_t pin) {
  bool changed = true;

  switch (currentScreen) {
    case SCREEN_TITLE:
      if (pin == PIN_RHT) currentScreen = SCREEN_LIST;
      else changed = false;
      break;

    case SCREEN_LIST:
      if (pin == PIN_RHT) {
        currentScreen = SCREEN_SETTINGS;
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

    case SCREEN_SETTINGS:
      if (pin == PIN_LET) currentScreen = SCREEN_LIST;
      else changed = false;
      break;
  }

  if (changed) drawCurrentScreen(currentScreen, tasks, taskCount, selectedIndex, scrollOffset);
}

void setup() {
  setupInput();
  setupDisplay();
  drawCurrentScreen(currentScreen, tasks, taskCount, selectedIndex, scrollOffset);
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
