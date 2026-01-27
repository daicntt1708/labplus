#include "mqtt_client.h"
#include "config.h"
#include <WiFi.h>
#include <PubSubClient.h>

static WiFiClient net;
static PubSubClient client(net);

void mqttInit() {
  client.setServer(MQTT_BROKER, MQTT_PORT);
  client.setBufferSize(2048);   // ⭐ FIX LỖI KHÔNG THẤY MQTT
}

bool mqttReady() {
  return client.connected();
}

void mqttLoop() {
  if (!client.connected()) {
    Serial.print("[MQTT] Connecting...");
    if (client.connect(DEVICE_ID)) {
      Serial.println(" connected");
    } else {
      Serial.print(" fail rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }
  client.loop();
}

void mqttPublishBatch(SensorPacket* p, int n) {
  String payload = "{ \"device\":\"gateway01\", \"batch\":[";

  for (int i = 0; i < n; i++) {
    payload += "{";
    payload += "\"temp\":" + String(p[i].temp) + ",";
    payload += "\"alarm\":" + String(p[i].alarm ? "true" : "false") + ",";
    payload += "\"ts\":" + String(p[i].ts);
    payload += "}";
    if (i < n - 1) payload += ",";
  }
  payload += "]}";

  if (client.publish(MQTT_TOPIC, payload.c_str())) {
    Serial.println("[MQTT] Batch published");
  } else {
    Serial.println("[MQTT] Publish FAILED");
  }
}
