#pragma once

#include <cstdint>

#include "../Modules/Amp.h"
#include "../Modules/Envelope.h"
#include "../Modules/Filter.h"
#include "../Modules/Lfo.h"
#include "../Modules/Oscillator.h"
#include "SynthParams.h"

struct SynthParams;

class Voice {
 public:
  // ------ Midi Inputs ------ //
  float note = 0.0f;
  float velocityNorm = 0.0f;  // [0,1]
  // ------ Modulator outputs ------ //
  float envFilterOut = 0.0f;
  float envModOut = 0.0f;
  float lfo1Out = 0.0f;
  float lfo2Out = 0.0f;

  float envAmpOut = 0.0f;

  SynthParams p;

  Oscillator osc1;
  Oscillator osc2;
  Filter filter;
  Amp amp;

  Envelope envAmp;
  Envelope envFilter;
  Envelope envMod;
  Lfo lfo1;
  Lfo lfo2;
  // This lfo belongs to voice[0] and is shared by all voices
  // Used for global vibrato
  Lfo lfoMono;

  void reset();

  void importParams(const SynthParams& newParams);
  void updateStaticParams();  // Update params with no modulation

  void randomizeLfoPhase(int seed);
  void advanceLfos();
  void advanceModEnvs();  // envMod and envFilter
  void updateControlRate();

  void setSampleRate(float newSampleRate);
  void startNote(float note, float velocity);
  void release();

  bool isActive() const;

  float render();

 private:
  float sampleRate = 44100.0f;

  float osc1period = 0.0f;  // Amount of samples in one cycle
  float osc2period = 0.0f;  // Amount of samples in one cycle
  float pitchModOctaves = 0.0f;

  float trackingCallibration = 0.0;  // Used to make keytrack fit note to hz curve when filter cutoff is at its min value

  float periodFromNote(float note, float semitone, float cents) const;
};
