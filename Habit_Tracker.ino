#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH   128
#define SCREEN_HEIGHT  64
#define OLED_RESET     -1
#define SCREEN_ADDRESS 0x3C

// 5D-Joystick pins — all on Port D (COM → GND, active LOW)
// D1 (TX) is used for UP, so Serial is unavailable
#define PIN_UP   1   // PD1
#define PIN_RST  2   // PD2
#define PIN_SET  3   // PD3
#define PIN_MID  4   // PD4
#define PIN_LFT  5   // PD5
#define PIN_RHT  6   // PD6
#define PIN_DWN  7   // PD7

#define DEBOUNCE_MS 150

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

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

#define HEADER_HEIGHT 12
#define ROW_HEIGHT    10
#define LIST_TOP      (HEADER_HEIGHT + 2)
#define VISIBLE_ROWS  ((SCREEN_HEIGHT - LIST_TOP) / ROW_HEIGHT)

int selectedIndex = 0;
int scrollOffset  = 0;

volatile uint8_t pressedPin = 0;

ISR(PCINT2_vect) {
  uint8_t portD = PIND;
  if      (!(portD & (1 << PD1))) pressedPin = PIN_UP;
  else if (!(portD & (1 << PD7))) pressedPin = PIN_DWN;
  else if (!(portD & (1 << PD5))) pressedPin = PIN_LFT;
  else if (!(portD & (1 << PD6))) pressedPin = PIN_RHT;
  else if (!(portD & (1 << PD4))) pressedPin = PIN_MID;
  else if (!(portD & (1 << PD3))) pressedPin = PIN_SET;
  else if (!(portD & (1 << PD2))) pressedPin = PIN_RST;
}

void drawScreen() {
  display.clearDisplay();

  // Header
  display.fillRect(0, 0, SCREEN_WIDTH, HEADER_HEIGHT, SSD1306_WHITE);
  display.setTextColor(SSD1306_BLACK);
  display.setTextSize(1);
  display.setCursor(4, 2);
  display.print("Habit Tracker");

  // Task list
  for (int i = 0; i < VISIBLE_ROWS; i++) {
    int taskIndex = scrollOffset + i;
    if (taskIndex >= taskCount) break;

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

void handleInput(uint8_t pin) {
  bool changed = false;

  switch (pin) {
    case PIN_UP:
      if (selectedIndex > 0) {
        selectedIndex--;
        if (selectedIndex < scrollOffset)
          scrollOffset = selectedIndex;
        changed = true;
      }
      break;

    case PIN_DWN:
      if (selectedIndex < taskCount - 1) {
        selectedIndex++;
        if (selectedIndex >= scrollOffset + VISIBLE_ROWS)
          scrollOffset = selectedIndex - VISIBLE_ROWS + 1;
        changed = true;
      }
      break;
  }

  if (changed) drawScreen();
}

void setup() {
  pinMode(PIN_RST, INPUT_PULLUP);
  pinMode(PIN_SET, INPUT_PULLUP);
  pinMode(PIN_MID, INPUT_PULLUP);
  pinMode(PIN_RHT, INPUT_PULLUP);
  pinMode(PIN_LFT, INPUT_PULLUP);
  pinMode(PIN_DWN, INPUT_PULLUP);
  pinMode(PIN_UP,  INPUT_PULLUP);

  // Enable PCINT for Port D, pins PD1–PD7
  PCICR  |= (1 << PCIE2);
  PCMSK2 |= (1 << PCINT17) | (1 << PCINT18) | (1 << PCINT19) | (1 << PCINT20)
           | (1 << PCINT21) | (1 << PCINT22) | (1 << PCINT23);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    for (;;);
  }

  drawScreen();
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
