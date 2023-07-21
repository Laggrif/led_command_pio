/*
#include <Arduino.h>
#include "RotaryEncoder.h"

  unsigned long _lastIncReadTime = micros();
  unsigned long _lastDecReadTime = micros();
  int _pauseLength = 25000;
  int _fastIncrement = 10;

  int counter = 0;

  int ENC_A;
  int ENC_B;

  RotaryEncoder(int ENC_A, int ENC_B) {
    this->ENC_A = ENC_A;
    this->ENC_B = ENC_B;
    pinMode(ENC_A, INPUT_PULLUP);
    pinMode(ENC_B, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(ENC_A), _readCountStatic, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENC_B), _readCountStatic, CHANGE);
  }

  int get_count() {
    return counter;
  }

private:
  static RotaryEncoder* encoderInstance; // Pointer to the instance of RotaryEncoder

  static void _readCountStatic() {
    if (encoderInstance) {
      RotaryEncoder* encoder = encoderInstance; // Dereference the pointer

      static uint8_t old_AB = 3;                     // Lookup table index
      static int8_t encval = 0;                      // Encoder value
      static const int8_t enc_states[] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0}; // Lookup table

      old_AB <<= 2;                                  // Remember previous state

      if (digitalRead(encoder->ENC_A)) old_AB |= 0x02;         // Add current state of pin A
      if (digitalRead(encoder->ENC_B)) old_AB |= 0x01;         // Add current state of pin B

      encval += enc_states[(old_AB & 0x0f)];

      // Update counter if encoder has rotated a full indent, that is at least 4 steps
      if (encval > 3) {                               // Four steps forward
        int changevalue = 1;
        if ((micros() - encoder->_lastIncReadTime) < encoder->_pauseLength) {
          changevalue = encoder->_fastIncrement * changevalue;
        }
        encoder->_lastIncReadTime = micros();
        encoder->counter = (encoder->counter + changevalue) % 12;        // Update counter
        encval = 0;
      } else if (encval < -3) {                       // Four steps backward
        int changevalue = -1;
        if ((micros() - encoder->_lastDecReadTime) < encoder->_pauseLength) {
          changevalue = encoder->_fastIncrement * changevalue;
        }
        encoder->_lastDecReadTime = micros();
        encoder->counter = (encoder->counter + changevalue) % 12;        // Update counter
        encval = 0;
      }
    }
  }
};

RotaryEncoder* RotaryEncoder::encoderInstance = nullptr; // Initialize the static member variable

RotaryEncoder::RotaryEncoder(int ENC_A, int ENC_B) {
  this->ENC_A = ENC_A;
  this->ENC_B = ENC_B;
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);
  encoderInstance = this; // Set the instance pointer to the current object
  attachInterrupt(digitalPinToInterrupt(ENC_A), _readCountStatic, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_B), _readCountStatic, CHANGE);
}
*/