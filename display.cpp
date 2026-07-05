#include "display.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

#define HEADER_HEIGHT 12
#define ROW_HEIGHT 10
#define LIST_TOP (HEADER_HEIGHT + 2)
#define VISIBLE_ROWS ((SCREEN_HEIGHT - LIST_TOP) / ROW_HEIGHT)

static Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setupDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    for (;;)
      ;
  }
}

static void drawHeader(const char *title) {
  display.fillRect(0, 0, SCREEN_WIDTH, HEADER_HEIGHT, SSD1306_WHITE);
  display.setTextColor(SSD1306_BLACK);
  display.setTextSize(1);
  display.setCursor(4, 2);
  display.print(title);
}

static void drawTitle() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(10, 16);
  display.print("Habit");
  display.setCursor(10, 36);
  display.print("Tracker");
  display.display();
}

static void drawList(const char *tasks[], int taskCount, int selectedIndex,
                     int scrollOffset) {
  display.clearDisplay();
  drawHeader("Habits");
  for (int i = 0; i < VISIBLE_ROWS; i++) {
    int taskIndex = scrollOffset + i;
    if (taskIndex >= taskCount)
      break;
    int y = LIST_TOP + i * ROW_HEIGHT;
    if (taskIndex == selectedIndex) {
      display.fillRect(0, y, SCREEN_WIDTH, ROW_HEIGHT, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK);
    } else {
      display.setTextColor(SSD1306_WHITE);
    }
    display.setCursor(4, y + 1);
    display.print(tasks[taskIndex]);
  }
  display.display();
}

static void drawSettings() {
  display.clearDisplay();
  drawHeader("Settings");
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(4, LIST_TOP + 1);
  display.print("Coming soon...");
  display.display();
}

void drawCurrentScreen(Screen screen, const char *tasks[], int taskCount,
                       int selectedIndex, int scrollOffset) {
  switch (screen) {
  case SCREEN_TITLE:
    drawTitle();
    break;
  case SCREEN_LIST:
    drawList(tasks, taskCount, selectedIndex, scrollOffset);
    break;
  case SCREEN_SETTINGS:
    drawSettings();
    break;
  }
}
