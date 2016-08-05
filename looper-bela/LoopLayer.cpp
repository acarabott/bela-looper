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
  recorded = false;
  startRecordingScheduled = false;
  stopRecordingScheduled = false;
  mul = 1.0;

  recordingStartedFrame = 0;
  recordingStoppedFrame = 0;
  loopEnd = numBufferFrames;
}

void LoopLayer::input(uint64_t clockFrame, float signal)
{
  if (recording) {
    // write(frame, signal);
  }
}

void LoopLayer::startRecording(uint64_t clockFrame)
{
  if (recording) {
    return;
  }

  recording = true;
  startRecordingScheduled = false;

  if (!recorded) {
    recordingStartedFrame = clockFrame;
  }

  #ifdef DEBUG
    rt_printf("starting recording - Layer %d\n", id);
  #endif
}

void LoopLayer::stopRecording(uint64_t clockFrame)
{
  if (!recording) {
    return;
  }

  recording = false;
  stopRecordingScheduled = false;

  if (!recorded) {
    recorded = true;
    recordingStoppedFrame = clockFrame;
  }

  #ifdef DEBUG
    rt_printf("stopped recording - Layer %d\n", id);
    rt_printf("\tnumLoopFrames: %d\n", numLoopFrames());
  #endif
}

void LoopLayer::toggleRecording(uint64_t clockFrame)
{
  if (recording) {
    stopRecording(clockFrame);
  } else {
    startRecording(clockFrame);
  }
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

float LoopLayer::read(uint64_t clockFrame)
{
  return 0;
}

void LoopLayer::write()
{

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
  recordingStartedFrame = 0;
  recordingStoppedFrame = 0;
  recorded = false;
  #ifdef DEBUG
    rt_printf("erased - Layer %d\n", id);
  #endif
}

uint32_t LoopLayer::numLoopFrames()
{
  return min(recordingStoppedFrame - recordingStartedFrame, numBufferFrames);
}

LoopLayer::~LoopLayer() {

}