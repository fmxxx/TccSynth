#include "Voice.h"

#include <algorithm>
#include <cmath>

#include "../Constants.h"

namespace {
// Remaps the raw shape parameter so a perfect saw exists in a small window
// around zero, and the morph ramps in smoothly outside it.
//
//   |shape| <= 0.05       -> 0 (perfect saw)
//   0.05 < |shape| < 0.25 -> cubic Hermite ramp
//   |shape| >= 0.25       -> input unchanged
//
// Cubic Hermite matches value and slope at both endpoints, so there is no
// audible kick-in at ±0.05 or at ±0.25.
inline float applyShapeDeadZone(float s) {
  constexpr float deadZone = 0.05f;
  constexpr float rampEnd = 0.25f;
  constexpr float rampWidth = rampEnd - deadZone;

  const float a = std::abs(s);
  if (a <= deadZone) return 0.0f;
  if (a >= rampEnd) return s;

  const float sign = (s > 0.0f) ? 1.0f : -1.0f;
  const float t = (a - deadZone) / rampWidth;
  const float t2 = t * t;
  const float t3 = t2 * t;

  // Cubic Hermite: f(0)=0, f(1)=rampEnd, f'(0)=0, f'(1)=rampWidth
  const float h = (-2.0f * t3 + 3.0f * t2) * rampEnd + (t3 - t2) * rampWidth;

  return sign * h;
}
}  // namespace

void Voice::reset() {
  note = 0;
  velocity = 0;
  osc1period = 0.0f;
  osc2period = 0.0f;
  pitchModOctaves = 0.0f;
  osc1.reset();
  osc2.reset();

  filter.reset();

  envAmp.reset();
  envFilter.reset();
  envMod.reset();

  lfo1.reset();
  lfo2.reset();
}

void Voice::updateParams(const SynthParams& newParams) {
  p = newParams;

  // --- Oscillators ---
  // Pitch is calculated from the note and these parameters when a note starts.
  osc1.shape = applyShapeDeadZone(p.osc1.shape);
  osc1.amplitude = p.osc1.volume / 100.0f;

  osc2.shape = applyShapeDeadZone(p.osc2.shape);
  osc2.amplitude = p.osc2.volume / 100.0f;

  // --- Filter ---
  filter.cutoffFreq = p.filter.cutoff;
  filter.resonance = p.filter.resonance / 100.0f;

  envFilter.attackMs = p.filter.attack;
  envFilter.decayMs = p.filter.decay;
  envFilter.sustain = p.filter.sustain;
  envFilter.releaseMs = p.filter.release;

  // --- Amp ---
  amp.volume = p.amp.volume / 100.0f;

  envAmp.velocitySens = p.amp.velocity / 100.0f;
  envAmp.attackMs = p.amp.attack;
  envAmp.decayMs = p.amp.decay;
  envAmp.sustain = p.amp.sustain;
  envAmp.releaseMs = p.amp.release;
  envAmp.attackFromZero = p.amp.attackFromZero;

  // --- Mod Envelope ---
  // voiceFreq handled in updateParams()
  envMod.attackMs = p.modEnv.attack;
  envMod.decayMs = p.modEnv.decay;
  envMod.sustain = p.modEnv.sustain;
  envMod.releaseMs = p.modEnv.release;

  // --- LFO 1 ---
  // TODO figure out how to handle voiceFreq

  lfo1.freq = p.lfo1.rate;
  lfo1.shape = p.lfo1.shape;
  lfo1.keytrig = p.lfo1.keytrig;
  lfo1.amplitude = p.lfo1.amplitude / 100.0f;

  // --- LFO 2 ---
  lfo2.freq = p.lfo2.rate;
  lfo2.shape = p.lfo2.shape;
  lfo2.keytrig = p.lfo2.keytrig;
  lfo2.amplitude = p.lfo2.amplitude / 100.0f;

  // After the assignments above:
  osc1period = periodFromNote(note, p.osc1.semitone, p.osc1.cents);
  osc2period = periodFromNote(note, p.osc2.semitone, p.osc2.cents);

  filter.updateParams();

  envAmp.updateParams();
  envFilter.updateParams();
  envMod.updateParams();

  lfo1.updateParams();
  lfo2.updateParams();
}

void Voice::advanceLfos() {
  lfo1Out = lfo1.nextSample();
  lfo2Out = lfo2.nextSample();
}

void Voice::updateSlowMods() {
  envFilterOut = envFilter.nextSample();
  envModOut = envMod.nextSample();

  // TODO apply modulation to parameters
  // --- Osc 1 --- //
  float osc1ShapeModSum = (p.modEnv.shape1 * envModOut + p.lfo1.shape1 * lfo1Out + p.lfo2.shape1 * lfo2Out);
  osc1ShapeModSum = osc1ShapeModSum * 0.01f;
  osc1.applyModulations(osc1ShapeModSum, 0);

  // --- Osc 2 --- //
  float osc2ShapeModSum = (p.modEnv.shape2 * envModOut + p.lfo1.shape2 * lfo1Out + p.lfo2.shape2 * lfo2Out);
  osc2ShapeModSum = osc2ShapeModSum * 0.01f;
  float osc2VolSum = p.modEnv.osc2Volume * 0.01f + envModOut;
  osc2VolSum = osc2VolSum * 0.01f;
  osc2.applyModulations(osc2ShapeModSum, osc2VolSum);

  // --- Filter --- //

  // --- Global --- //

  // Voice pitch
  constexpr float LFO_OCT_PER_UNIT = 0.001667f;
  constexpr float ENV_OCT_PER_UNIT = 0.01f;

  pitchModOctaves =
      p.lfo1.voiceFreq * LFO_OCT_PER_UNIT * lfo1Out +
      p.lfo2.voiceFreq * LFO_OCT_PER_UNIT * lfo2Out +
      p.modEnv.voiceFreq * ENV_OCT_PER_UNIT * envModOut;
}

void Voice::setSampleRate(float newSampleRate) {
  sampleRate = newSampleRate;

  filter.setsampleRate(newSampleRate);

  lfo1.setSampleRate(newSampleRate);
  lfo2.setSampleRate(newSampleRate);

  envAmp.setSampleRate(newSampleRate);

  // Make modulation envelopes have a slower sample rate (updated every
  // CONTROL_RATE_SAMPLES instead of every sample)
  const float controlRate = newSampleRate / static_cast<float>(CONTROL_RATE_SAMPLES);
  envFilter.setSampleRate(controlRate);
  envMod.setSampleRate(controlRate);
}

void Voice::startNote(float newNote, float newVelocity) {
  note = newNote;
  velocity = newVelocity;
  envAmp.velocityNorm = velocity / 127.0f;

  osc1period = periodFromNote(note, p.osc1.semitone, p.osc1.cents);
  osc2period = periodFromNote(note, p.osc2.semitone, p.osc2.cents);

  osc1.newNote();
  osc2.newNote();

  if (lfo1.keytrig) {
    lfo1Out = 0.0f;
    lfo1.setPhase(0.0f);
  }

  if (lfo2.keytrig) {
    lfo2Out = 0.0f;
    lfo1.setPhase(0.0f);
  }

  envAmp.start();
  envFilter.start();
  envMod.start();
}

void Voice::release() {
  envAmp.release();
  envFilter.release();
  envMod.release();
}

float Voice::render() {
  // If pitchRatio = 1, then frequency doesnt change from midi note's
  const float pitchRatio = std::exp2(pitchModOctaves);

  // phaseInc = 1 * pitchRatio / osc1period; where 1 is a full cycle
  osc1.phaseInc = pitchRatio / osc1period;
  osc2.phaseInc = pitchRatio / osc2period;

  float oscMix = osc1.nextSample() + osc2.nextSample();
  oscMix = oscMix * 0.5;

  float filterOut = filter.nextSample(oscMix);

  float envOut = amp.nextSample(filterOut, envAmp.nextSample());

  return envOut;
}

bool Voice::isActive() const {
  return envAmp.isActive();
}

float Voice::periodFromNote(float note, float semitone, float cents) const {
  const float semitonesFromA4 = (note - 69.0f) + semitone + (cents / 100.0f);
  const float noteFrequencyHz = p.global.tuning * std::exp2(semitonesFromA4 / 12.0f);

  return sampleRate / noteFrequencyHz;
}
