#include "Voice.h"

#include <cmath>

#include "Synth.h"

void Voice::reset() {
  note = 0;
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
  // Pitch information will be read by float periodFromNote()
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

  // Modulation sum
  osc2.volMod = p.modEnv.osc2Volume;
  osc1.shapeMod = p.modEnv.shape1 + p.lfo1.shape1 + p.lfo2.shape1;
  osc2.shapeMod = p.modEnv.shape2 + p.lfo2.shape2 + p.lfo2.shape2;
  filter.cutoffFreqMod = p.lfo1.cutoffFreq + p.lfo2.cutoffFreq;

  // After the assignments above:
  osc1.updateParams();
  osc2.updateParams();
  filter.updateParams();
  amp.updateParams();
  envAmp.updateParams();
  envFilter.updateParams();
  envMod.updateParams();
  lfo1.updateParams();
  lfo2.updateParams();
}

void Voice::updateLfos() {
  envFilterOut = envFilter.nextSample();
  envModOut = envMod.nextSample();

  lfo1Out = lfo1.nextSample();
  lfo2Out = lfo2.nextSample();

  // TODO apply modulation to parameters
}

void Voice::start(float midiNote, int midiVelocity, float sampleRate) {
  float periodOsc1 = periodFromNote(sampleRate, midiNote, p.global.tuning, p.osc1.semitone, p.osc1.cents);

  float periodOsc2 = periodFromNote(sampleRate, midiNote, p.global.tuning, p.osc2.semitone, p.osc2.cents);

  osc1.phaseInc = periodOsc1;
  osc1.newNote();

  osc2.phaseInc = periodOsc2;
  osc2.newNote();

  envAmp.start();
}

void Voice::release() {
  envAmp.release();
  envFilter.release();
  envMod.release();
}

float Voice::render() {
  float oscMix = osc1.nextSample() + osc2.nextSample();

  float filterOut = filter.nextSample(oscMix);

  float envOut = amp.nextSample(filterOut);

  return envOut;
}

float periodFromNote(float sampleRate, float midiNote, float globalTuningA4, float semitone, float cents) {
  float semitonesFromA4 = (midiNote - 69.0f) + semitone + (cents / 100.0f);
  float freqHz = globalTuningA4 * std::exp2(semitonesFromA4 / 12.0f);

  return sampleRate / freqHz;
}