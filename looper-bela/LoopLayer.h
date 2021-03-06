#ifndef LOOP_LAYER
#define LOOP_LAYER

#include <stdint.h>

class LoopLayer
{
public:
  LoopLayer();
  ~LoopLayer();

  static const uint32_t numBufferFrames = 1323000; // 30 seconds
  void input(uint64_t clockFrame, float signal);
  void startRecording(uint64_t clockFrame);
  void stopRecording(uint64_t clockFrame);
  void toggleRecording(uint64_t clockFrame);
  void scheduleRecordingStart();
  void scheduleRecordingStop();
  bool recordingStartScheduled();
  bool recordingStopScheduled();
  bool isRecording();
  float read(uint64_t clockFrame);
  void write(uint32_t bufferFrame, float signal);
  float getMul();
  void setMul(float _mul);
  void erase();

protected:
  uint16_t id;
  float buffer[numBufferFrames];
  bool recording;
  bool recorded;
  float mul;
  bool startRecordingScheduled;
  bool stopRecordingScheduled;

  uint64_t recordingStartedFrame;
  uint64_t recordingStoppedFrame;

  uint32_t loopEnd;
  uint32_t numLoopFrames();
  uint64_t getBufferIndex(uint64_t clockFrame);

private:
  static uint16_t idGenerator;

};

#endif