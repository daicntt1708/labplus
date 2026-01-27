// buffer.cpp
#include "buffer.h"
#include <deque>
static std::deque<SensorPacket> buf;

void bufferPush(const SensorPacket& pkt) { buf.push_back(pkt); }
bool bufferEmpty() { return buf.empty(); }
bool bufferPop(SensorPacket& pkt) {
  if (buf.empty()) return false;
  pkt = buf.front(); buf.pop_front();
  return true;
}
