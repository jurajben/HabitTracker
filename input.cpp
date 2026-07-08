#include "input.h"
#include <Arduino.h>
#include <avr/interrupt.h>

volatile uint8_t pressedPin = 0;

ISR(PCINT2_vect) {
  uint8_t portD = PIND;
  if (!(portD & (1 << PD1)))
    pressedPin = PIN_UP;
  else if (!(portD & (1 << PD0)))
    pressedPin = PIN_DWN;
  else if (!(portD & (1 << PD3)))
    pressedPin = PIN_LET;
  else if (!(portD & (1 << PD4)))
    pressedPin = PIN_RHT;
  else if (!(portD & (1 << PD5)))
    pressedPin = PIN_MID;
  else if (!(portD & (1 << PD6)))
    pressedPin = PIN_SET;
  else if (!(portD & (1 << PD7)))
    pressedPin = PIN_RST;
}

void setupInput() {
  pinMode(PIN_UP, INPUT_PULLUP);
  pinMode(PIN_DWN, INPUT_PULLUP);
  pinMode(PIN_LET, INPUT_PULLUP);
  pinMode(PIN_RHT, INPUT_PULLUP);
  pinMode(PIN_MID, INPUT_PULLUP);
  pinMode(PIN_SET, INPUT_PULLUP);
  pinMode(PIN_RST, INPUT_PULLUP);

  PCICR |= (1 << PCIE2);
  PCMSK2 |= (1 << PCINT16) | (1 << PCINT17) | (1 << PCINT19) | (1 << PCINT20) |
            (1 << PCINT21) | (1 << PCINT22) | (1 << PCINT23);
}
