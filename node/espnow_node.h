#pragma once

#include "packet.h"

/* init ESP-NOW */
void espnowInit();

/* send packet to gateway */
bool espnowSend(const SensorPacket& pkt);
