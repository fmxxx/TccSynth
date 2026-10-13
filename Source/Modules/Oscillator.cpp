
#include "Oscillator.h"

#include <math.h>

#include <algorithm>

// Eff means "effective", the value after all modulation is applied
Oscillator::Oscillator()
    : shape(0.0f),
      amplitude(1.0f),
      phaseInc(0.0f),
      shapeEff(0.0f),
      ampEff(1.0f),
      phase(0.0f) {}

// --- --- Control-rate updates (called by the Voice) --- --- //

void Oscillator::applyModulations(float shapeMod, float volMod) {
  shapeEff = std::clamp(shape + shapeMod, -1.0f, 1.0f);
  ampEff = std::clamp(amplitude + volMod, 0.0f, 1.0f);
}

// --- --- Lifecycle --- --- //

void Oscillator::reset() {
  phase = 0.0f;
}

void Oscillator::newNote() {
  reset();
}

// --- --- DSP --- --- //

float Oscillator::poly_blep(float phase, float phaseInc) {
  if (phaseInc <= 0.0f) return 0.0f;

  if (phase < phaseInc) {
    // Just after the discontinuity at phase 0.
    const float t = phase / phaseInc;
    return t + t - t * t - 1.0f;
  } else if (phase > 1.0f - phaseInc) {
    // Just before the discontinuity at phase 1.
    const float t = (phase - 1.0f) / phaseInc;
    return t * t + t + t + 1.0f;
  }
  return 0.0f;
}

float Oscillator::saw(float p, float dt) {
  const float naive = 2.0f * p - 1.0f;
  return naive - poly_blep(p, dt);
}

float Oscillator::triangleShape(float v1, float slant) {
  v1 = std::clamp(v1, -1.0f, 1.0f);
  if (slant < 0.5f) slant = 0.5f;
  if (slant > 1.0f) slant = 1.0f;
  if (slant >= 1.0f) return v1;

  // The two line segments of the piecewise fold. Outside its domain,
  // each segment grows without bound; the soft minimum below prevents
  // that from leaking into the output.
  const float b1 = (v1 + 1.0f) / slant - 1.0f;
  const float b2 = (1.0f - v1) / (1.0f - slant) - 1.0f;

  // Corner softening radius. Smaller = sharper triangle, more aliasing.
  // Larger = rounder peak, less aliasing. 0.02 .. 0.10 is a useful range.
  const float r = 0.1f;

  const float diff = b1 - b2;
  return 0.5f * (b1 + b2 - std::sqrt(diff * diff + r * r));
}

// --- --- Per-sample --- --- //

float Oscillator::nextSample() {
  const float dt = phaseInc;
  float out = 0.0f;

  if (shapeEff < 0.0f) {
    // ----- Saw to triangle morph -----
    // slant: 1.0 at shape = 0, 0.5 at shape = -1.
    const float slant = 1.0f + 0.5f * shapeEff;
    const float sawA = 2.0f * phase - 1.0f;
    out = triangleShape(sawA, slant);
  } else {
    // ----- Saw -> square -> narrow pulse -----
    const float sawA = saw(phase, dt);

    float mix = 0.0f;
    float offsetB = 0.0f;
    float add = 0.0f;

    if (shapeEff <= 0.5f) {
      // Fade SawB in; at shape = 0.5 the two saws cancel into a square.
      mix = shapeEff * 2.0f;  // 0 .. 1
      offsetB = 0.5f;         // B is half a cycle behind A
      add = 0.0f;
    } else {
      // Shift SawB further to narrow the high-duty pulse.
      // offsetB: 0.5 (50% duty) .. 0.01 (1% high duty).
      mix = 1.0f;
      offsetB = 0.5f - (shapeEff - 0.5f) * 0.98f;
      add = 2.0f * offsetB - 1.0f;  // DC offset to keep output centered
    }

    float phaseB = phase + offsetB;
    if (phaseB >= 1.0f) phaseB -= 1.0f;

    const float sawB = saw(phaseB, dt);

    out = sawA - mix * sawB + mix * add;
  }

  // Advance and wrap the phase.
  phase += phaseInc;
  if (phase >= 1.0f) phase -= 1.0f;
  if (phase < 0.0f) phase += 1.0f;

  out *= ampEff;
  return out;
}