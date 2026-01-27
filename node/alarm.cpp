// alarm.cpp
#include "alarm.h"
#include "config.h"
#include <Arduino.h>

static bool alarmState = false;
static unsigned long lastBlink = 0;
static bool led = false;

void alarmInit() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

void alarmOn() { alarmState = true; }
void alarmOff() {
  alarmState = false;
  digitalWrite(LED_PIN, LOW);
}

void alarmLoop() {
  if (!alarmState) return;
  if (millis() - lastBlink > 500) {
    led = !led;
    digitalWrite(LED_PIN, led);
    lastBlink = millis();
  }
}
