#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

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

  float envAmpOut = 0.0f;  // Needs to be public so Synth can find quietest amp for voice allocation

  SynthParams p;

  void reset();

  // ------ Parameter Stuff ------ //
  void importParams(const SynthParams& newParams);
  void updateStaticParams();  // Update params with no modulation

  void randomizeLfoPhase(int seed);

  void setSampleRate(float newSampleRate);

  // ------ Note lifecycle ------ //
  void startNote(float note, float velocity);
  void release();

  // ------ Envelope getters ------ //
  bool isAttack() const;
  bool isActive() const;

  // ------ Control Rate ------ //
  void tickControlRate();

  // ------ Every Sample ------ //
  float render();

 private:
  float sampleRate = 44100.0f;

  float osc1period = 0.0f;  // Amount of samples in one cycle
  float osc2period = 0.0f;  // Amount of samples in one cycle
  float pitchModOctaves = 0.0f;

  float trackingCallibration = 0.0;  // Used to make keytrack fit note to hz curve when filter cutoff is at its min value

  // ------ Smoothed parameters ------ //
  juce::SmoothedValue<float> ampVolSmoother;
  juce::SmoothedValue<float> osc1VolSmoother;
  juce::SmoothedValue<float> osc2VolSmoother;
  juce::SmoothedValue<float> osc1ShapeSmoother;
  juce::SmoothedValue<float> osc2ShapeSmoother;
  juce::SmoothedValue<float> filterCutoffSmoother;
  juce::SmoothedValue<float> filterResSmoother;
  // LFO amplitudes — consumed at CONTROL rate, reset() with control rate
  juce::SmoothedValue<float> lfo1VolSmoother;
  juce::SmoothedValue<float> lfo2VolSmoother;

  // Audio rate
  juce::SmoothedValue<float> pitchSmoother;
  // ------ Old parameters ------ //
  float oldGlideSecs = 0.0f;

  // ------ Modulator outputs ------ //
  float envFilterOut = 0.0f;
  float envModOut = 0.0f;
  float lfo1Out = 0.0f;
  float lfo2Out = 0.0f;

  // ------ Modules ------ //
  Oscillator osc1;
  Oscillator osc2;
  Filter filter;
  Amp amp;

  Envelope envAmp;
  Envelope envFilter;
  Envelope envMod;
  Lfo lfo1;
  Lfo lfo2;

  // ------ Control Rate ------ //
  void advanceLfos();
  void advanceModEnvs();  // envMod and envFilter
  void applyModulations();

  // ------ Helpers ------ //
  float periodFromNote(float note, float semitone, float cents) const;
};
