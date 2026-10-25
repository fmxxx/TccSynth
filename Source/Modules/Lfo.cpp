
#include "Lfo.h"

#include <JuceHeader.h>

#include <cmath>
#include <iostream>

#include "../Constants.h"

void Lfo::reset() {
}

void Lfo::setPhase(float newPhase) {
  phase = newPhase;
}

void Lfo::updateParams() {
  phaseInc = (freq * CONTROL_RATE_SAMPLES) / sampleRate;
}

void Lfo::setSampleRate(float newSampleRate) {
  sampleRate = newSampleRate;
}
float Lfo::nextSample() {
  float sample = 0.0f;

  switch (shape) {
    case 0:  // Sine
      sample = std::sin(phase * 2.0f * M_PI);
      break;
    case 1:  // Tri (0 at phase = 0)
      float p = phase + 0.25f;
      p -= std::floor(p);
      sample = 1.0f - 4.0f * std::abs(p - 0.5f);
      break;
    case 2:  // Saw (falling, +1 -> -1)
      sample = 1.0f - 2.0f * phase;
      break;
    case 3:  // Ramp (rising, -1 -> +1)
      sample = 2.0f * phase - 1.0f;
      break;
    case 4:  // Square
      sample = (phase < 0.5f) ? 1.0f : -1.0f;
      break;
    case 5:  // 25% Pulse
      sample = (phase < 0.25f) ? 1.0f : -1.0f;
      break;
    case 6:  // 10% Pulse
      sample = (phase < 0.1f) ? 1.0f : -1.0f;
      break;
  }

  // Advance
  phase += phaseInc;
  if (phase >= 1.0f) {
    phase -= 1.0f;
  }

  return sample * amplitude;
}