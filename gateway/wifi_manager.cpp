#include <Arduino.h>     
#include <WiFi.h>
#include "wifi_manager.h"
#include "config.h"
#include <WebServer.h>

static WebServer server(80);

/* ================= WEB PAGE ================= */

void handleRoot() {
  server.send(200, "text/html",
    "<h3>ESP32 Gateway WiFi Config</h3>"
    "<form action='/save'>"
    "SSID:<br><input name='s'><br>"
    "Password:<br><input name='p' type='password'><br><br>"
    "<input type='submit' value='Save'>"
    "</form>"
    "<br><hr>"
    "<p>STA status: " + String(WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected") + "</p>"
    "<p>IP: " + WiFi.localIP().toString() + "</p>"
  );
}

void handleSave() {
  String ssid = server.arg("s");
  String pass = server.arg("p");

  Serial.println("[WIFI] New WiFi config received");
  Serial.print("SSID: ");
  Serial.println(ssid);

  WiFi.disconnect();      // CHỈ NGẮT STA
  delay(300);

  WiFi.begin(ssid.c_str(), pass.c_str());

  server.send(200, "text/plain",
    "Saved!\n"
    "Gateway is connecting to new WiFi.\n"
    "You can keep this page open.");
}

/* ================= AP INIT ================= */

void startAP() {
  Serial.println("[WIFI] Starting AP config portal");

  WiFi.softAP(AP_SSID, AP_PASS);

  IPAddress ip = WiFi.softAPIP();
  Serial.print("[WIFI] AP IP: ");
  Serial.println(ip);

  server.on("/", handleRoot);
  server.on("/save", handleSave);
  server.begin();
}

/* ================= WIFI INIT ================= */

void wifiInit() {
  Serial.println("[WIFI] Init AP + STA");

  WiFi.mode(WIFI_AP_STA);   // ⭐ QUAN TRỌNG

  startAP();                // AP LUÔN BẬT
  WiFi.begin();             // dùng WiFi đã lưu trong flash

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED &&
         millis() - start < 8000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("[WIFI] STA connected, IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("[WIFI] STA not connected (AP only)");
  }
}

/* ================= LOOP ================= */

void wifiLoop() {
  server.handleClient();

  static unsigned long lastRetry = 0;
  if (millis() - lastRetry > WIFI_RETRY_INTERVAL) {
    lastRetry = millis();

    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("[WIFI] STA disconnected → reconnect");
      WiFi.reconnect();
    }
  }
}

bool wifiIsConnected() {
  return WiFi.status() == WL_CONNECTED;
}
