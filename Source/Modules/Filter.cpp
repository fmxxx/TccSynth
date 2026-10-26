
#include "Filter.h"

#include <algorithm>
#include <cmath>

Filter::Filter() {
  updateParams();
}

void Filter::reset() {
  s1 = 0.0f;
  s2 = 0.0f;
  s3 = 0.0f;
  s4 = 0.0f;
}

void Filter::setsampleRate(float newSampleRate) {
  sampleRate = std::max(newSampleRate, 1.0f);
  updateParams();
}

void Filter::updateParams() {
  cutoffFreq = std::isfinite(cutoffFreq) ? cutoffFreq : 20000.0f;
  cutoffFreq = std::clamp(cutoffFreq, 1.0f, sampleRate * 0.49f);
  resonance = std::isfinite(resonance) ? resonance : 0.0f;
  resonance = std::clamp(resonance, 0.0f, 1.0f);

  calcCoefs();
}

void Filter::calcCoefs() {
  g = std::tan(M_PI * cutoffFreq / sampleRate);
  G = g / (1.0f + g);
  G4 = G * G * G * G;
  k = 4.0f * resonance;

  const float oneMinusG = 1.0f - G;
  c1 = G * G * G * oneMinusG;
  c2 = G * G * oneMinusG;
  c3 = G * oneMinusG;
  c4 = oneMinusG;

  oneOverOnePlusKG4 = 1.0f / (1.0f + k * G4);
}

float Filter::nextSample(float input) {
  const float filtered = (G4 * input + c1 * s1 + c2 * s2 + c3 * s3 + c4 * s4) * oneOverOnePlusKG4;
  const float stageInput = input - k * filtered;

  const float y1 = G * stageInput + (1.0f - G) * s1;
  const float y2 = G * y1 + (1.0f - G) * s2;
  const float y3 = G * y2 + (1.0f - G) * s3;
  const float y4 = G * y3 + (1.0f - G) * s4;

  s1 = 2.0f * y1 - s1;
  s2 = 2.0f * y2 - s2;
  s3 = 2.0f * y3 - s3;
  s4 = 2.0f * y4 - s4;

  return filtered;
}
