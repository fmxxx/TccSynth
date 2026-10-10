
#include "Amp.h"

float Amp::nextSample(float input, float envSample) {
  float output = input * envSample;

  if (output > 1.0f) output = 1.0f;
  if (output < -1.0f) output = -1.0f;

  return output;
}