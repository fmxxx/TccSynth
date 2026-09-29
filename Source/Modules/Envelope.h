
#pragma once

#include <algorithm>
#include <cmath>

enum class EnvelopeStage {
  OFF,
  ATTACK,
  DECAY,
  SUSTAIN,
  RELEASE
};

class Envelope {
 public:
  // ------ Exposed Parameters ------ //
  float attackMs;
  float decayMs;
  float sustain;  //[0, 1]
  float releaseMs;
  float velocitySens;  // 0: Always low, 1: Vel 1 = near silence
  bool attackFromZero = false;

  // ------ Modulations ------ //
  float attackMsMod;
  float decayMsMod;
  float sustainMod;  //[0, 1]
  float releaseMsMod;

  void reset();
  void updateParams();
  void setsampleRate(float newsampleRate);
  void start();
  void release();
  float nextSample();

  // ------ Getters ------ //
  float getCurrentValue() const;
  EnvelopeStage getStage() const;
  bool isActive() const;

 private:
  float sampleRate;

  EnvelopeStage currentStage;
  float currentValue;

  float attackCoef = 0.0f;
  float decayCoef = 0.0f;
  float releaseCoef = 0.0f;
  float attackTarget = 1.5f;

  float sustainLevel;

  // Converts milliseconds to a single-sample RC filter coefficient
  float calculateCoef(float ms);
};
