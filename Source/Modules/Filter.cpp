
#include "Filter.h"

void Filter::reset() {
  s1 = 0.0f;
  s2 = 0.0f;
  s3 = 0.0f;
  s4 = 0.0f;
}

void Filter::updateParams() {}

void Filter::setsampleRate(float newSampleRate) {
  sampleRate = newSampleRate;
}
float Filter::nextSample(float input) {
  // TODO: Add filter
  return input;
}