// This class contains the variables for each parameter
// To be used in Synth.cpp and passed down to the voices

#pragma once

struct SynthParams {
  struct {
    float semitones = 0.0f,
          cents = 0.0f,
          shapeFctr = 0.0f,
          volPct = 100.0f;
    bool doKeytrig = true;
  } osc1, osc2;

  struct {
    float attackSecs = 0.1f,
          decaySecs = 1.0f,
          sustainFctr = 0.5f,
          releaseSecs = 1.0f;
    float cutoffHz = 20000.0f,
          resonancePct = 0.0f,
          keytrackPct = 0.0f,
          envAmountHz = 0.0f;
  } filter;

  struct {
    float attackSecs = 0.1f,
          decaySecs = 0.5f,
          sustainFctr = 0.5f,
          releaseSecs = 0.5f;
    bool doAttackFromZero = false;
    float velSensPct = 50.0f,
          volPct = 80.0f;
  } amp;

  struct {
    float hz = 1.0f;
    int waveformId = 0;
    bool doKeytrig = false;
    float volPct = 100.0f,
          cutoffHz = 0.0f,
          voiceFreqPct = 0.0f,
          shape1Pct = 0.0f,
          shape2Pct = 0.0f;
  } lfo1, lfo2;

  struct {
    float attackSecs = 0.1f,
          decaySecs = 0.5f,
          sustainFctr = 0.5f,
          releaseSecs = 0.5f;
    float osc2VolPct = 0.0f,
          voiceFreqPct = 0.0f,
          shape1Pct = 0.0f,
          shape2Pct = 0.0f;
  } modEnv;

  struct {
    float shape1Pct = 0.0f,
          shape2Pct = 0.0f,
          cutoffHz = 0.0f,
          resonancePct = 0.0f;
  } vel;

  struct {
    int voiceCount = 8;
    float glideSecs = 0.0f;
    float tuningA4Hz = 440;
  } global;
};
