#include "display.h"
#include "rtc.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

#define HEADER_HEIGHT 11
#define ROW_HEIGHT 10
#define LIST_TOP (HEADER_HEIGHT + 2)
#define VISIBLE_ROWS ((SCREEN_HEIGHT - LIST_TOP) / ROW_HEIGHT)

static Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setupDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    for (;;);
  }
}

static void drawHeader(const char* title) {
  display.fillRect(0, 0, SCREEN_WIDTH, HEADER_HEIGHT, SSD1306_WHITE);
  display.setTextColor(SSD1306_BLACK);
  display.setTextSize(1);
  display.setCursor(4, 2);
  display.print(title);
  DateTime now = rtc.now();
  char date[6];
  sprintf(date, "%02d/%02d", now.day(), now.month());
  display.setCursor(SCREEN_WIDTH - 34, 2);
  display.print(date);
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

static void drawList(const Task tasks[], int taskCount, int selectedIndex,
                     int scrollOffset) {
  display.clearDisplay();
  drawHeader("Habits");
  for (int i = 0; i < VISIBLE_ROWS; i++) {
    int taskIndex = scrollOffset + i;
    if (taskIndex >= taskCount) break;
    int y = LIST_TOP + i * ROW_HEIGHT;
    bool selected = (taskIndex == selectedIndex);
    bool completed = taskGetCompleted(tasks, taskIndex);
    uint16_t fg = selected ? SSD1306_BLACK : SSD1306_WHITE;
    if (selected) {
      display.fillRect(0, y, SCREEN_WIDTH, ROW_HEIGHT, SSD1306_WHITE);
    }
    char name[TASK_NAME_LEN];
    taskGetName(tasks, taskIndex, name);
    display.setTextColor(fg);
    display.setCursor(4, y + 1);
    display.print(name);
    int cx = SCREEN_WIDTH - 7, cy = y + ROW_HEIGHT / 2;
    if (completed) {
      display.fillCircle(cx, cy, 3, fg);
    } else {
      display.drawCircle(cx, cy, 3, fg);
    }
  }
  display.display();
}

static void drawTaskGraph() {
  display.clearDisplay();
  drawHeader("Task Graph");
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(4, LIST_TOP + 1);
  display.print("Coming soon...");
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

static int drawWrapped(const char* text, int x, int y, int charsPerLine,
                       int maxLines) {
  int len = strlen(text);
  int start = 0, line = 0;
  while (start < len && line < maxLines) {
    int end = start + charsPerLine;
    if (end < len) {
      int wb = end;
      while (wb > start && text[wb] != ' ') wb--;
      if (wb > start) end = wb;
    } else {
      end = len;
    }
    display.setCursor(x, y + line * 8);
    for (int i = start; i < end; i++) display.print(text[i]);
    start = end;
    if (start < len && text[start] == ' ') start++;
    line++;
  }
  return line;
}

void drawDetailOverlay(const Task tasks[], int selectedIndex) {
  Task t;
  taskGet(tasks, selectedIndex, &t);

  const int WX = 4, WY = HEADER_HEIGHT + 4, WW = 120, WH = 64 - HEADER_HEIGHT - 6;

  display.fillRect(WX, WY, WW, WH, SSD1306_WHITE);
  display.fillRect(WX + 1, WY + 1, WW - 2, WH - 2, SSD1306_BLACK);

  display.fillRect(WX + 1, WY + 1, WW - 2, 10, SSD1306_WHITE);
  display.setTextSize(1);
  display.setTextColor(SSD1306_BLACK);
  display.setCursor(WX + 4, WY + 2);
  display.print(t.name);

  display.fillCircle(WX + WW - 7, WY + 5, 4, SSD1306_BLACK);
  if (taskCompleted[selectedIndex])
    display.fillCircle(WX + WW - 7, WY + 5, 2, SSD1306_WHITE);

  display.setTextColor(SSD1306_WHITE);

  display.setCursor(WX + 4, WY + 12);
  display.print(t.required ? "req." : "opt.");
  
  char timeStr[10];
  sprintf(timeStr, "rem:%02d:%02d", t.reminder.hour, t.reminder.minute);
  display.setCursor(WX + WW - (9 * 6) - 4, WY + 12);
  display.print(timeStr);
  display.drawFastHLine(WX, WY + 20, WW, SSD1306_WHITE);
  
  display.setCursor(WX + 4, WY + 22);
  drawWrapped(t.description, WX + 4, WY + 22, 17, 3);

  display.display();
}

void drawReminderScreen(const Task tasks[], int taskCount, uint8_t hour, uint8_t minute) {
  display.clearDisplay();
  drawHeader("Reminder:");
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  int y = LIST_TOP;
  int found = 0;
  Task t;
  for (int i = 0; i < taskCount && y < SCREEN_HEIGHT; i++) {
    taskGet(tasks, i, &t);
    if (t.reminder.hour == hour && t.reminder.minute == minute) {
      display.setCursor(4, y + 1);
      display.print(t.name);
      y += ROW_HEIGHT;
      found++;
    }
  }

  if (found == 0) {
    display.setCursor(4, LIST_TOP + 1);
    display.print("Time to check in!");
  }

  display.display();
}

void drawMessage(const char *line1, const char *line2) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(4, 20);
  display.print(line1);
  if (line2) {
    display.setCursor(4, 32);
    display.print(line2);
  }
  display.display();
}

void drawCurrentScreen(Screen screen, const Task tasks[], int taskCount,
                       int selectedIndex, int scrollOffset) {
  switch (screen) {
    case SCREEN_TITLE:
      drawTitle();
      break;
    case SCREEN_LIST:
      drawList(tasks, taskCount, selectedIndex, scrollOffset);
      break;
    case SCREEN_GRAPH:
      drawTaskGraph();
      break;
    case SCREEN_SETTINGS:
      drawSettings();
      break;
  }
}
