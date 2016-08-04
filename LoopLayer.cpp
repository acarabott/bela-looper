#include "LoopLayer.h"
#include <Utilities.h>
#include <algorithm>

LoopLayer::LoopLayer()
{
  erase();
  recording = false;
  mul = 1.0;
}

void LoopLayer::startRecording()
{
  recording = true;
}

void LoopLayer::stopRecording()
{
  recording = false;
}

void LoopLayer::toggleRecording()
{
  if (recording) {
    stopRecording();
  } else {
    startRecording();
  }
}

bool LoopLayer::recordEnabled()
{
  return recording;
}

float LoopLayer::read(uint32_t index)
{
  if (index >= BUFFER_SIZE) {
    return 0;
  }

  return buffer[index] * mul;
}

void LoopLayer::write(uint32_t index, float sample)
{
  if (index >= BUFFER_SIZE) {
    rt_printf("ERROR: write index out of bounds\n");
    return;
  }

  float summed = constrain(sample, -1, 1) + buffer[index];
  buffer[index] = constrain(summed, -1, 1);
}

float LoopLayer::getMul()
{
  return mul;
}

void LoopLayer::setMul(float _mul)
{
  mul = constrain(_mul, 0, 1);
}

void LoopLayer::erase()
{
  std::fill(buffer, buffer + BUFFER_SIZE, 0);
}

LoopLayer::~LoopLayer() {

}