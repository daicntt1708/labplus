// buffer.h
#pragma once
#include "packet.h"
bool bufferPop(SensorPacket& pkt);
void bufferPush(const SensorPacket& pkt);
bool bufferEmpty();
