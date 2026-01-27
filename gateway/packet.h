#pragma once
#include <Arduino.h>

struct SensorPacket {
  float temp;
  bool alarm;
  unsigned long ts;
};
