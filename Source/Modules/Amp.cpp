
#include "Amp.h"

#include <algorithm>

// ------ Per sample ------ //
float Amp::nextSample(float input, float envLevel) {
  // --- Apply amp envelope --- //
  output = input * envLevel;

  // --- Apply volume --- //
  output = output * volume;

  return std::clamp(output, -1.0f, 1.0f);
}