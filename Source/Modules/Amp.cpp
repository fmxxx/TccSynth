
#include "Amp.h"

#include <algorithm>

float Amp::nextSample(float input, float envLevel) {
  return std::clamp(input * envLevel, -1.0f, 1.0f);
}