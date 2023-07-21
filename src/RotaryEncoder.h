/*
#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#include <Arduino.h>

class RotaryEncoder {
public:
  unsigned long _lastIncReadTime = micros();
  unsigned long _lastDecReadTime = micros();
  int _pauseLength = 25000;
  int _fastIncrement = 10;

  int counter = 0;

  int ENC_A;
  int ENC_B;

  RotaryEncoder(int ENC_A, int ENC_B);

  int get_count();

private:
  void _readCount();
};

extern RotaryEncoder* encoderInstance;

#endif
*/