
#include "Filter.h"

void Filter::reset() {
  s1 = 0.0f;
  s2 = 0.0f;
  s3 = 0.0f;
  s4 = 0.0f;
}

float Filter::nextSample(float input) {
  // TODO: Add filter
  return input;
}