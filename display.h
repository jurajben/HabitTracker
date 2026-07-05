#pragma once

enum Screen { SCREEN_TITLE, SCREEN_LIST, SCREEN_SETTINGS };

void setupDisplay();
void drawCurrentScreen(Screen screen, const char *tasks[], int taskCount,
                       int selectedIndex, int scrollOffset);
