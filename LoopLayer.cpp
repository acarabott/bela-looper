#include "LoopLayer.h"
#include <Utilities.h>
#include <algorithm>

#define DEBUG

uint16_t LoopLayer::idGenerator = 0;

LoopLayer::LoopLayer()
{
  id = idGenerator++;
  erase();
  recording = false;
  startRecordingScheduled = false;
  stopRecordingScheduled = false;
  mul = 1.0;
}

void LoopLayer::startRecording()
{
  recording = true;
  startRecordingScheduled = false;

  #ifdef DEBUG
    rt_printf("starting recording - Layer %d\n", id);
  #endif
}

void LoopLayer::stopRecording()
{
  recording = false;
  stopRecordingScheduled = false;
  #ifdef DEBUG
    rt_printf("stopped recording - Layer %d\n", id);
  #endif
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

void LoopLayer::scheduleRecordingStart()
{
  startRecordingScheduled = true;
  #ifdef DEBUG
    rt_printf("recording scheduled - Layer %d\n", id);
  #endif
}

void LoopLayer::scheduleRecordingStop()
{
  stopRecordingScheduled = true;
  #ifdef DEBUG
    rt_printf("stopping scheduled - Layer %d\n", id);
  #endif
}

bool LoopLayer::recordingStartScheduled()
{
  return startRecordingScheduled;
}

bool LoopLayer::recordingStopScheduled()
{
  return stopRecordingScheduled;
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
  #ifdef DEBUG
    rt_printf("mul set - Layer %d: %f\n", id, mul);
  #endif
}

void LoopLayer::erase()
{
  std::fill(buffer, buffer + BUFFER_SIZE, 0);
  #ifdef DEBUG
    rt_printf("erased - Layer %d\n", id);
  #endif
}

LoopLayer::~LoopLayer() {

}