
#include "Amp.h"

#include <algorithm>

float Amp::nextSample(float input, float envLevel) {
  // --- Apply amp envelope --- //
  output = input * envLevel;

  return std::clamp(output, -1.0f, 1.0f);
}