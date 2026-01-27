#include "config.h"
#include "packet.h"
#include "sensor.h"
#include "alarm.h"
#include "espnow_node.h"

unsigned long lastSend = 0;

void setup() {
  Serial.begin(115200);
  alarmInit();
  espnowInit();
}

void loop() {
  alarmLoop();

  if (millis() - lastSend >= SEND_INTERVAL) {
    lastSend = millis();

    float temp = readTemperature();
    if (temp < -50) return;

    bool alarm = (temp < TEMP_MIN || temp > TEMP_MAX);
    if (alarm) alarmOn();
    else alarmOff();

    SensorPacket pkt;
    pkt.temp = temp;
    pkt.alarm = alarm;
    pkt.ts = millis();

    bool ok = espnowSend(pkt);

    Serial.printf("[NODE] T=%.1f alarm=%d espnow=%s\n",
                  temp, alarm, ok ? "OK" : "FAIL");
  }
}
