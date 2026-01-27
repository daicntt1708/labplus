#include <Arduino.h>

#include "config.h"
#include "packet.h"

#include "wifi_manager.h"
#include "espnow_gateway.h"
#include "mqtt_client.h"
#include "buffer.h"

/* ---------- BATCH BUFFER ---------- */
static SensorPacket batch[BATCH_SIZE];
static int batchCount = 0;

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println("\n=== ESP32 GATEWAY START ===");

  /* =====================================================
   * 1️⃣ WIFI INIT
   * - WIFI_AP_STA
   * - AP LUÔN BẬT để cấu hình
   * - STA kết nối WiFi nếu có
   * - QUYẾT ĐỊNH CHANNEL cho ESP-NOW
   * ===================================================== */
  wifiInit();

  /* =====================================================
   * 2️⃣ ESP-NOW INIT
   * - BẮT BUỘC SAU wifiInit()
   * - Dùng channel hiện tại của STA
   * ===================================================== */
  espnowInit();

  /* =====================================================
   * 3️⃣ MQTT INIT
   * - Không block nếu chưa có WiFi
   * - mqttLoop() chỉ chạy khi WiFi OK
   * ===================================================== */
  mqttInit();
}

void loop() {
  /* =====================================================
   * WIFI + AP CONFIG PORTAL
   * - Luôn gọi để web config hoạt động
   * ===================================================== */
  wifiLoop();

  /* =====================================================
   * MQTT LOOP
   * - Chỉ chạy khi STA connected
   * ===================================================== */
  if (wifiIsConnected()) {
    mqttLoop();
  }

  /* =====================================================
   * ESP-NOW RECEIVE
   * - KHÔNG phụ thuộc WiFi / MQTT
   * ===================================================== */
  if (espnowHasPacket()) {
    SensorPacket pkt = espnowGetPacket();

    Serial.println("[GATEWAY] ESP-NOW packet received");

    batch[batchCount++] = pkt;

    /* ---------------- BATCH FULL ---------------- */
    if (batchCount >= BATCH_SIZE) {

      if (wifiIsConnected() && mqttReady()) {
        mqttPublishBatch(batch, batchCount);
        Serial.println("[MQTT] Batch published");
      } else {
        for (int i = 0; i < batchCount; i++) {
          bufferPush(batch[i]);
        }
        Serial.println("[BUFFER] Batch buffered (offline)");
      }

      batchCount = 0;
    }
  }

  /* =====================================================
   * FLUSH BUFFER
   * - Khi WiFi + MQTT OK thì đẩy dữ liệu tồn
   * ===================================================== */
  if (wifiIsConnected() && mqttReady()) {
    while (!bufferEmpty()) {
      SensorPacket pkt;
      if (bufferPop(pkt)) {
        SensorPacket one[1] = { pkt };
        mqttPublishBatch(one, 1);
      }
    }
  }

  delay(5);  // chống watchdog
}
