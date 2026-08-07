#include "buzzer.h"
#include <Arduino.h>

void setupBuzzer() {
  pinMode(BUZZER_PIN, OUTPUT);
}

void buzzReminder() {
  tone(BUZZER_PIN, 1047, 150); delay(200);
  tone(BUZZER_PIN, 1319, 150); delay(200);
  tone(BUZZER_PIN, 1568, 300);
}
