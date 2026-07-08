#pragma once
#include "tasks.h"

enum Screen { SCREEN_TITLE, SCREEN_LIST, SCREEN_GRAPH, SCREEN_SETTINGS };

void setupDisplay();
void drawCurrentScreen(Screen screen, const Task tasks[], int taskCount,
                       int selectedIndex, int scrollOffset);
