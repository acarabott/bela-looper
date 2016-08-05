#include "LoopLayer.h"
#include <Utilities.h>
#include <algorithm>

#define DEBUG
#ifdef DEBUG
    #define __STDC_FORMAT_MACROS
    #include <inttypes.h>
#endif

uint16_t LoopLayer::idGenerator = 0;

LoopLayer::LoopLayer()
{
  id = idGenerator++;
  erase();
  recording = false;
  startRecordingScheduled = false;
  stopRecordingScheduled = false;
  mul = 1.0;

  setLoopStartFrame(0);
  setLoopEndFrame(numBufferFrames);
}

void LoopLayer::startRecording(uint64_t currentFrame)
{
  recording = true;
  startRecordingScheduled = false;
  setLoopStartFrame(currentFrame);

  #ifdef DEBUG
    rt_printf("starting recording - Layer %d\n", id);
  #endif
}

void LoopLayer::stopRecording(uint64_t currentFrame)
{
  recording = false;
  stopRecordingScheduled = false;
  setLoopEndFrame(currentFrame);

  #ifdef DEBUG
    rt_printf("stopped recording - Layer %d\n", id);
  #endif
}

void LoopLayer::toggleRecording(uint64_t currentFrame)
{
  if (recording) {
    stopRecording(currentFrame);
  } else {
    startRecording(currentFrame);
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

float LoopLayer::read(uint64_t currentFrame)
{
  uint64_t offsetFrame = currentFrame - loopEndFrame;
  uint32_t numLoopFrames = loopEndFrame - loopStartFrame;
  uint32_t bufferFrame = offsetFrame % numLoopFrames;
  return buffer[bufferFrame] * mul;
}

void LoopLayer::write(uint64_t currentFrame, float sample)
{
  uint32_t bufferFrame = (currentFrame - loopStartFrame) % numBufferFrames;
  float summed = constrain(sample, -1, 1) + buffer[bufferFrame];
  buffer[bufferFrame] = constrain(summed, -1, 1);

  if (currentFrame % 4410 == 0) {
    rt_printf("writing @ %d\n", bufferFrame);
  }
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
  std::fill(buffer, buffer + numBufferFrames, 0);
  setLoopStartFrame(0);
  setLoopEndFrame(numBufferFrames);
  #ifdef DEBUG
    rt_printf("erased - Layer %d\n", id);
  #endif
}

void LoopLayer::setLoopStartFrame(uint64_t startFrame)
{
  loopStartFrame = startFrame;

  #ifdef DEBUG
    rt_printf("loop start set - Layer %d: %d\n", id, loopStartFrame);
  #endif
}

void LoopLayer::setLoopEndFrame(uint64_t endFrame)
{
  loopEndFrame = max(loopStartFrame, endFrame);

  #ifdef DEBUG
    rt_printf("loop end set - Layer %d: %d\n", id, loopEndFrame);
  #endif
}

LoopLayer::~LoopLayer() {

}