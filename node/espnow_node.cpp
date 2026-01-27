#include "espnow_node.h"

#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

/* 🔴 WIFI CỦA GATEWAY (CHỈ DÙNG ĐỂ LẤY CHANNEL) */
#define WIFI_SSID "Z"
#define WIFI_PASS "L1532000"

/* 🔴 MAC GATEWAY (BẠN ĐÃ CUNG CẤP) */
uint8_t gatewayMac[] = {0x94, 0x51, 0xDC, 0x34, 0x78, 0xF4};

static bool sendOK = false;

/* ===== CALLBACK CHUẨN ESP-IDF 5.x ===== */
void onSend(const wifi_tx_info_t* info,
            esp_now_send_status_t status) {
  sendOK = (status == ESP_NOW_SEND_SUCCESS);
}

void espnowInit() {
  /* 1️⃣ Node KHÔNG CẦN INTERNET – CHỈ CẦN CHANNEL */
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.print("[NODE] Joining WiFi to get channel");
  unsigned long t0 = millis();
  while (WiFi.status() != WL_CONNECTED &&
         millis() - t0 < 6000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  int channel = WiFi.channel();
  Serial.printf("[NODE] Home channel = %d\n", channel);

  if (channel == 0) {
    Serial.println("[NODE] ERROR: Channel = 0, abort ESP-NOW");
    return;
  }

  /* 2️⃣ Init ESP-NOW */
  if (esp_now_init() != ESP_OK) {
    Serial.println("[NODE] ESP-NOW init failed");
    return;
  }

  esp_now_register_send_cb(onSend);

  /* 3️⃣ Add gateway peer */
  esp_now_peer_info_t peer = {};
  memcpy(peer.peer_addr, gatewayMac, 6);
  peer.channel = channel;     // 🔥 TRÙNG HOME CHANNEL
  peer.encrypt = false;

  if (esp_now_add_peer(&peer) == ESP_OK) {
    Serial.println("[NODE] Peer added");
  } else {
    Serial.println("[NODE] Add peer FAILED");
  }

  /* 4️⃣ Ngắt WiFi INTERNET nhưng GIỮ RADIO */
  WiFi.disconnect(false);
}

bool espnowSend(const SensorPacket& pkt) {
  sendOK = false;

  esp_err_t err = esp_now_send(
    gatewayMac,
    (uint8_t*)&pkt,
    sizeof(pkt)
  );

  if (err != ESP_OK) {
    Serial.printf("[NODE] Send error: %d\n", err);
    return false;
  }

  delay(10);  // chờ callback
  return sendOK;
}
