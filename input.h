#pragma once
#include <stdint.h>

#define PIN_UP  1   // PD1
#define PIN_DWN 8   // PD0 — identifier 8 to avoid conflicting with the no-press sentinel (0)
#define PIN_LET 3
#define PIN_RHT 4
#define PIN_MID 5
#define PIN_SET 6
#define PIN_RST 7

#define DEBOUNCE_MS 150

extern volatile uint8_t pressedPin;

void setupInput();
