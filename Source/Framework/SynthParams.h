// This class contains the variables for each parameter
// To be used in Synth.cpp and passed down to the voices

#pragma once

struct SynthParams {
  struct {
    float semitone = 0.0f,
          cents = 0.0f,
          shape = 0.0f,
          volume = 100.0f;
  } osc1, osc2;

  struct {
    float attack = 0.1f,
          decay = 1.0f,
          sustain = 0.5f,
          release = 1.0f;
    float cutoff = 20000.0f,
          resonance = 0.0f,
          keytrack = 0.0f,
          envAmount = 0.0f;
  } filter;

  struct {
    float attack = 0.01f,
          decay = 0.5f,
          sustain = 0.5f,
          release = 0.5f;
    bool attackFromZero = false;
    float velocity = 50.0f,
          volume = 80.0f;
  } amp;

  struct {
    float rate = 1.0f;
    int shape = 0;
    bool keytrig = false;
    float amplitude = 0.0f,
          cutoffFreq = 0.0f,
          voiceFreq = 0.0f,
          shape1 = 0.0f,
          shape2 = 0.0f;
  } lfo1, lfo2;

  struct {
    float attack = 0.01f,
          decay = 0.5f,
          sustain = 0.5f,
          release = 0.5f;
    float osc2Volume = 0.0f,
          voiceFreq = 0.0f,
          shape1 = 0.0f,
          shape2 = 0.0f;
  } modEnv;

  struct {
    float shape1 = 0.0f,
          shape2 = 0.0f,
          cutoff = 0.0f,
          resonance = 0.0f;
  } vel;

  struct {
    int voiceCount = 8;
    float glide = 0.0f;
    float tuning = 440;
  } global;
};
