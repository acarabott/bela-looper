#ifndef LOOP_LAYER
#define LOOP_LAYER

#include <stdint.h>

#define BUFFER_SIZE 1323000 // 30 seconds

class LoopLayer
{
protected:
  float buffer[BUFFER_SIZE];
  bool recording;
  float mul;

public:
  LoopLayer();
  ~LoopLayer();

  void startRecording();
  void stopRecording();
  void toggleRecording();
  bool recordEnabled();
  float read(uint32_t index);
  void write(uint32_t index, float sample);
  float getMul();
  void setMul(float _mul);
  void erase();
};

#endif