
#include "Amp.h"

float Amp::nextSample(float input, float envLevel) {
  float output = input * envLevel;

  if (output > 1.0f) output = 1.0f;
  if (output < -1.0f) output = -1.0f;

  return output;
}