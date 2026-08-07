#pragma once
#include "tasks.h"

enum Screen { SCREEN_TITLE, SCREEN_LIST, SCREEN_GRAPH, SCREEN_SETTINGS };

void setupDisplay();
void drawCurrentScreen(Screen screen, const Task tasks[], int taskCount,
                       int selectedIndex, int scrollOffset);
void drawDetailOverlay(const Task tasks[], int selectedIndex);
void drawReminderScreen(const Task tasks[], int taskCount, uint8_t hour, uint8_t minute);
void drawMessage(const char *line1, const char *line2 = nullptr);
