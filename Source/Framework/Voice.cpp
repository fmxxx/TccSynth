#include "Voice.h"

#include <algorithm>
#include <cmath>

#include "../Constants.h"

void Voice::reset() {
  note = 0;
  velocity = 0;
  period1 = 0.0f;
  period2 = 0.0f;
  pitchModSemitones = 0.0f;
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
  osc1.shape = p.osc1.shape;
  osc1.amplitude = p.osc1.volume / 100.0f;

  osc2.shape = p.osc2.shape;
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
  amp.velocitySens = p.amp.velocity / 100.0f;

  envAmp.attackMs = p.amp.attack;
  envAmp.decayMs = p.amp.decay;
  envAmp.sustain = p.amp.sustain;
  envAmp.releaseMs = p.amp.release;
  envAmp.attackFromZero = p.amp.attackFromZero;

  // --- Mod Envelope ---
  // TODO figure out how to handle voiceFreq
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
  filter.updateParams();

  envAmp.updateParams();
  envFilter.updateParams();
  envMod.updateParams();

  lfo1.updateParams();
  lfo2.updateParams();
}

void Voice::updateSlowMods() {
  lfo1Out = lfo1.nextSample();
  lfo2Out = lfo2.nextSample();
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
  float osc2VolSum = p.modEnv.osc2Volume + envModOut;
  osc2VolSum = osc2VolSum * 0.01f;
  osc2.applyModulations(osc2ShapeModSum, osc2VolSum);

  // --- Filter --- //

  // --- Global --- //
  const float voiceFreqModSum =
      p.lfo1.voiceFreq * lfo1Out + p.lfo2.voiceFreq * lfo2Out + p.modEnv.voiceFreq * envModOut;
  const float normalizedPitchMod = std::clamp(voiceFreqModSum * 0.003f, -1.0f, 1.0f);
  pitchModSemitones = normalizedPitchMod * 12.0f;
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
  period1 = periodFromNote(note, p.osc1.semitone, p.osc1.cents);
  period2 = periodFromNote(note, p.osc2.semitone, p.osc2.cents);

  osc1.newNote();
  osc2.newNote();

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
  const float frequency1Hz = sampleRate / period1 * std::exp2(pitchModSemitones / 12.0f);
  const float frequency2Hz = sampleRate / period2 * std::exp2(pitchModSemitones / 12.0f);

  osc1.phaseInc = frequency1Hz / sampleRate;
  osc2.phaseInc = frequency2Hz / sampleRate;

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
