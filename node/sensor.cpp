// sensor.cpp
#include "sensor.h"
#include "config.h"
#include <DHT.h>

static DHT dht(DHT_PIN, DHT_TYPE);
static bool inited = false;

float readTemperature() {
  if (!inited) {
    dht.begin();
    inited = true;
  }
  float t = dht.readTemperature();
  if (isnan(t)) return -100;
  return t;
}
