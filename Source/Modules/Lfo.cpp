
#include "Lfo.h"

#include <cmath>
#include <iostream>

#include "../Framework/Synth.h"  // Only for the CONTROL_RATE_SAMPLES

void Lfo::reset() {}

void Lfo::updateParams() {
  phaseInc = (2.0f * M_PI * freq * CONTROL_RATE_SAMPLES) / sampleRate;
}

void Lfo::setSampleRate(float newSampleRate) {
  sampleRate = newSampleRate;
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
