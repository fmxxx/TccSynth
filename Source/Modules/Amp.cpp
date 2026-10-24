
#include "Amp.h"

#include <algorithm>

float Amp::nextSample(float input, float envLevel) {
  // --- Apply amp envelope --- //
  output = input * envLevel;

  // --- Velocity Scaling --- //
  output = output * (velocityNorm * velocitySens + (1 - velocitySens));

  return std::clamp(output, -1.0f, 1.0f);
}