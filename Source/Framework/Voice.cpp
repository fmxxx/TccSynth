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

  filter.cutoffFreqModAmt = p.lfo1.cutoffFreq + p.lfo2.cutoffFreq;

  // After the assignments above:
  // osc1.updateParams();
  // osc2.updateParams();
  filter.updateParams();
  amp.updateParams();
  envAmp.updateParams();
  envFilter.updateParams();
  envMod.updateParams();
  lfo1.updateParams();
  lfo2.updateParams();
}

void Voice::updateSlowMods() {
  envFilterOut = envFilter.nextSample();
  envModOut = envMod.nextSample();

  lfo1Out = lfo1.nextSample();
  lfo2Out = lfo2.nextSample();

  // TODO apply modulation to parameters
  // --- Osc 1 --- //
  float osc1ShapeModSum = (p.modEnv.shape1 * envModOut + p.lfo1.shape1 * lfo1Out + p.lfo2.shape1 * lfo2Out);
  osc1.applyModulations(osc1ShapeModSum, 0);

  // --- Osc 2 --- //
  float osc2ShapeModSum = (p.modEnv.shape2 * envModOut + p.lfo1.shape2 * lfo1Out + p.lfo2.shape2 * lfo2Out);
  float osc2VolSum = p.modEnv.osc2Volume * envModOut;
  osc2.applyModulations(osc2ShapeModSum, osc2VolSum);

  // --- Filter --- //

  // --- Global --- //
  float voiceFreqModSum = (p.lfo1.voiceFreq * lfo1Out + p.lfo2.voiceFreq * lfo2Out + p.modEnv.voiceFreq * envModOut);
  // TODO: Apply modulation to voice by changing the A4 freq
}

void Voice::start(float note, int velocity) {
  float periodOsc1 = periodFromNote(sampleRate, note, p.global.tuning, p.osc1.semitone, p.osc1.cents);

  float periodOsc2 = periodFromNote(sampleRate, note, p.global.tuning, p.osc2.semitone, p.osc2.cents);

  // TODO: Figure out how implement the period function so it produces a pitch
  osc1.newNote();

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
// How many samples for the wave to go through one cycle
float periodFromNote(float sampleRate, float note, float globalTuningA4, float semitone, float cents) {
  float semitonesFromA4 = (note - 69.0f) + semitone + (cents / 100.0f);
  float freqHz = globalTuningA4 * std::exp2(semitonesFromA4 / 12.0f);

  return sampleRate / freqHz;
}