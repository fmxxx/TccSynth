#pragma once

#include <cstdint>

#include "../Modules/Amp.h"
#include "../Modules/Envelope.h"
#include "../Modules/Filter.h"
#include "../Modules/Lfo.h"
#include "../Modules/Oscillator.h"
#include "SynthParams.h"

struct SynthParams;

float periodFromNote(float sampleRate, float midiNote, float globalTuningA4, float semitone, float cents);

class Voice {
 public:
  // ------ Midi Inputs ------ //
  float note;
  float velocity;
  // ------ Internal State ------ //
  bool isActive;
  float period;
  float target;
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

  void reset();

  void updateParams(const SynthParams& newParams);
  void updateLfos();

  void start(float midiNote, int midiVelocity, float sampleRate);
  void release();

  float render();

 private:
  // Returns freq in Hz
  float freqFromNote(float sampleRate, float globalTuning, float semitone, float cents);
};
