#include "Voice.h"

#include <JuceHeader.h>

#include <algorithm>
#include <cmath>

#include "../Constants.h"
#include "../Utils.h"

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
  velocityNorm = 0.0f;
  osc1period = 0.0f;
  osc2period = 0.0f;
  pitchModOctaves = 0.0f;
  osc1.reset();
  osc2.reset();

  filter.reset();

  envAmp.reset();
  envFilter.reset();
  envMod.reset();

  envFilterOut = 0.0f;
  envModOut = 0.0f;

  lfo1.reset();  // Does nothing atm
  lfo2.reset();  // Does nothing atm

  lfo1Out = 0.0f;
  lfo2Out = 0.0f;
}

void Voice::importParams(const SynthParams& newParams) {
  p = newParams;
}

void Voice::updateStaticParams() {
  // --- Oscillators ---
  // Only recalulate period if semi or cents change
  if (p.osc1.semitones != prevOsc1Semi || p.osc1.cents != prevOsc1Cent) {
    osc1period = periodFromNote(note, p.osc1.semitones, p.osc1.cents);
  }
  prevOsc1Semi = p.osc1.semitones;
  prevOsc1Cent = p.osc1.cents;
  // osc1.shape is modulated
  osc1.amplitude = p.osc1.volPct * 0.01f;

  // Only recalulate period if semi or cents change
  if (p.osc2.semitones != prevOsc2Semi || p.osc2.cents != prevOsc2Cent) {
    osc2period = periodFromNote(note, p.osc2.semitones, p.osc2.cents);
  }
  prevOsc2Semi = p.osc2.semitones;
  prevOsc2Cent = p.osc2.cents;
  // osc2.shape is modulated
  // osc2.amplitude is modulated

  // --- Filter ---
  // filter.cutoffFreq is modulated
  // filter.resonance is modulated

  envFilter.attackMs = p.filter.attackSecs * 1000;
  envFilter.decayMs = p.filter.decaySecs * 1000;
  envFilter.sustain = p.filter.sustainFctr;
  envFilter.releaseMs = p.filter.releaseSecs * 1000;
  envFilter.updateParams();

  // --- Amp ---
  amp.volume = p.amp.volPct * 0.01f;
  envAmp.velocitySens = p.amp.velSensPct * 0.01f;
  envAmp.attackFromZero = p.amp.doAttackFromZero;

  envAmp.attackMs = p.amp.attackSecs * 1000;
  envAmp.decayMs = p.amp.decaySecs * 1000;
  envAmp.sustain = p.amp.sustainFctr;
  envAmp.releaseMs = p.amp.releaseSecs * 1000;
  envAmp.updateParams();

  // --- Mod Envelope ---
  envMod.attackMs = p.modEnv.attackSecs * 1000;
  envMod.decayMs = p.modEnv.decaySecs * 1000;
  envMod.sustain = p.modEnv.sustainFctr;
  envMod.releaseMs = p.modEnv.releaseSecs * 1000;
  envMod.updateParams();

  // --- LFO 1 ---
  lfo1.freq = p.lfo1.hz;
  lfo1.shape = p.lfo1.waveformId;
  lfo1.keytrig = p.lfo1.doKeytrig;
  lfo1.amplitude = p.lfo1.volPct * 0.01f;
  lfo1.updateParams();

  // --- LFO 2 ---
  lfo2.freq = p.lfo2.hz;
  lfo2.shape = p.lfo2.waveformId;
  lfo2.keytrig = p.lfo2.doKeytrig;
  lfo2.amplitude = p.lfo2.volPct * 0.01f;
  lfo2.updateParams();

  // --- Global ---
  // Used to tune the keytracked filter to same hz as oscillators
  // 20.0f is the lowest filter cutoff, make sure to update it
  // if ParamaterList changes
  trackingCallibration = p.global.tuningA4Hz / (20.0f * std::exp2(69.0f / 12.0f));
}

void Voice::randomizeLfoPhase(int seed) {
  lfo1.setPhase(randomHash(seed));
  // Offset so no two voices have same seed
  lfo2.setPhase(randomHash(seed + 128));
}

void Voice::advanceLfos() {
  lfo1Out = lfo1.nextSample();
  lfo2Out = lfo2.nextSample();
}

void Voice::advanceModEnvs() {
  envFilterOut = envFilter.nextSample();
  envModOut = envMod.nextSample();
}

void Voice::updateControlRate() {
  // --- Osc 1 --- //
  float osc1ShapeModSum = (p.modEnv.shape1Pct * envModOut +
                           p.lfo1.shape1Pct * lfo1Out +
                           p.lfo2.shape1Pct * lfo2Out +
                           p.vel.shape1Pct * velocityNorm) *
                          0.01f;

  osc1.shape = std::clamp(applyShapeDeadZone(p.osc1.shapeFctr) +
                              osc1ShapeModSum,
                          -1.0f, 1.0f);

  // --- Osc 2 --- //
  float osc2ShapeModSum = (p.modEnv.shape2Pct * envModOut +
                           p.lfo1.shape2Pct * lfo1Out +
                           p.lfo2.shape2Pct * lfo2Out +
                           p.vel.shape2Pct * velocityNorm) *
                          0.01f;

  osc2.shape = std::clamp(applyShapeDeadZone(p.osc2.shapeFctr) +
                              osc2ShapeModSum,
                          -1.0f, 1.0f);

  float osc2VolModSum = (p.modEnv.osc2VolPct * 0.01f * envModOut);

  osc2.amplitude = std::clamp(p.osc2.volPct * 0.01f + osc2VolModSum, 0.0f, 1.0f);

  // --- Filter --- //
  // Keytracking behavior: when cutoff is at its min value and keytracking = 1
  // the filter should be perfectly in tune with the keyboard
  float k = p.filter.keytrackPct * 0.01f;
  float f = p.filter.cutoffHz;
  float cutoffKeytracked = f * std::exp2(k * note / 12.0f) * trackingCallibration;

  float cutoffModSum = (p.filter.envAmountHz * envFilterOut +
                        p.lfo1.cutoffHz * lfo1Out +
                        p.lfo2.cutoffHz * lfo2Out +
                        p.vel.cutoffHz * velocityNorm);

  filter.cutoffFreq = std::clamp(cutoffKeytracked + cutoffModSum, 20.0f, 20000.0f);

  float resonanceModSum = (p.vel.resonancePct * 0.01f * envModOut);

  filter.resonance = std::clamp(p.filter.resonancePct * 0.01f + resonanceModSum, 0.0f, 1.0f);

  // --- Voice pitch --- //
  constexpr float LFO_OCT_PER_UNIT = 0.001667f;
  constexpr float ENV_OCT_PER_UNIT = 0.01f;

  pitchModOctaves =
      p.lfo1.voiceFreqPct * LFO_OCT_PER_UNIT * lfo1Out +
      p.lfo2.voiceFreqPct * LFO_OCT_PER_UNIT * lfo2Out +
      p.modEnv.voiceFreqPct * ENV_OCT_PER_UNIT * envModOut;

  // --- Update modules ONLY AFTER ALL MODs ARE CALCULATED --- //
  filter.updateParams();
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
  velocityNorm = newVelocity / 127.0f;
  envAmp.velocityNorm = velocityNorm;

  osc1period = periodFromNote(note, p.osc1.semitones, p.osc1.cents);
  osc2period = periodFromNote(note, p.osc2.semitones, p.osc2.cents);

  osc1.newNote();
  osc2.newNote();

  if (lfo1.keytrig) {
    lfo1Out = 0.0f;
    lfo1.setPhase(0.0f);
  }

  if (lfo2.keytrig) {
    lfo2Out = 0.0f;
    lfo2.setPhase(0.0f);
  }

  envAmp.start();
  envFilter.start();
  envMod.start();

  updateControlRate();
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

float Voice::periodFromNote(float note, float semitones, float cents) const {
  const float semitonesFromA4 = (note - 69.0f) + semitones + (cents / 100.0f);
  const float noteFrequencyHz = p.global.tuningA4Hz * std::exp2(semitonesFromA4 / 12.0f);

  return sampleRate / noteFrequencyHz;
}
