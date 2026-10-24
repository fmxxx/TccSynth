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
  float velocity = 0.0f;
  // ------ Modulator outputs ------ //
  float envFilterOut = 0.0f;
  float envModOut = 0.0f;
  float lfo1Out = 0.0f;
  float lfo2Out = 0.0f;

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

  void updateParams(const SynthParams& newParams);
  void updateSlowMods();

  void setSampleRate(float newSampleRate);
  void startNote(float note, float velocity);
  void release();

  bool isActive() const;

  float render();

 private:
  float sampleRate = 44100.0f;

  float osc1period = 0.0f;  // Amount of samples in one cycle
  float osc2period = 0.0f;  // Amount of samples in one cycle
  float pitchModSemitones = 0.0f;

  float periodFromNote(float note, float semitone, float cents) const;
};
