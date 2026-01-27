#include "espnow_gateway.h"
#include <WiFi.h>
#include <esp_now.h>

static SensorPacket lastPkt;
static bool hasPkt = false;

void onRecv(const esp_now_recv_info* info,
            const uint8_t* data, int len) {
  if (len == sizeof(SensorPacket)) {
    memcpy(&lastPkt, data, sizeof(SensorPacket));
    hasPkt = true;
    Serial.println("[ESP-NOW] Packet received");
  }
}

void espnowInit() {
  Serial.printf("[ESP-NOW] Init on channel %d\n", WiFi.channel());

  if (esp_now_init() != ESP_OK) {
    Serial.println("[ESP-NOW] Init failed");
    return;
  }

  esp_now_register_recv_cb(onRecv);
}

bool espnowHasPacket() {
  return hasPkt;
}

SensorPacket espnowGetPacket() {
  hasPkt = false;
  return lastPkt;
}
