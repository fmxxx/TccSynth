
#include "Lfo.h"

#include <cmath>
#include <iostream>

void Lfo::reset() {}

void Lfo::updateParams() {
  phaseInc = getPeriodFromFreq(freq, sampleRate);
}

float Lfo::nextSample() {
  // TODO: add other shapes
  // If shape = sine
  float sample = std::sin(phase);

  phase += phaseInc;

  if (phase >= 2.0f * M_PI) {
    phase -= 2.0f * M_PI;
  }

  return sample;
}

float Lfo::getPeriodFromFreq(float freq, float sampleRate) {
  return (2.0f * M_PI * freq) / sampleRate;
}