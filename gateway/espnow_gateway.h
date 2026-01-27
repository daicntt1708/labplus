// espnow_gateway.h
#pragma once
#include "packet.h"

void espnowInit();
bool espnowHasPacket();
SensorPacket espnowGetPacket();
