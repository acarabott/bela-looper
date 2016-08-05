#ifndef LOOP_LAYER
#define LOOP_LAYER

#include <stdint.h>

class LoopLayer
{
public:
  LoopLayer();
  ~LoopLayer();

  static const uint32_t numBufferFrames = 1323000; // 30 seconds
  void startRecording(uint64_t currentFrame);
  void stopRecording(uint64_t currentFrame);
  void toggleRecording(uint64_t currentFrame);
  bool recordEnabled();
  void scheduleRecordingStart();
  void scheduleRecordingStop();
  bool recordingStartScheduled();
  bool recordingStopScheduled();
  float read(uint64_t currentFrame);
  void write(uint64_t currentFrame, float sample);
  float getMul();
  void setMul(float _mul);
  void erase();

protected:
  uint16_t id;
  float buffer[numBufferFrames];
  bool recording;
  float mul;
  bool startRecordingScheduled;
  bool stopRecordingScheduled;
  uint64_t loopStartFrame;
  uint64_t loopEndFrame;

  void setLoopStartFrame(uint64_t startFrame);
  void setLoopEndFrame(uint64_t endFrame);

private:
  static uint16_t idGenerator;

};

#endif