// mqtt_client.h
#pragma once
#include "packet.h"

void mqttInit();
void mqttLoop();
bool mqttReady();
void mqttPublishBatch(SensorPacket* p, int n);
