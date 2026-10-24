
#pragma once

#include <algorithm>
#include <cmath>

enum class EnvelopeStage {
  OFF,
  DECLICK,
  ATTACK,
  DECAY,
  SUSTAIN,
  RELEASE
};

class Envelope {
 public:
  // ------ Exposed Parameters ------ //
  float attackMs = 0.0f;
  float decayMs = 0.0f;
  float sustain = 0.0f;  //[0, 1]
  float releaseMs = 0.0f;
  bool attackFromZero = false;
  float velocitySens = 0.0f;  // [0,1], Only used by envAmp

  float velocityNorm = 0.0f;  // [0,1]

  // ------ Modulations ------ //
  float attackMsMod = 0.0f;
  float decayMsMod = 0.0f;
  float sustainMod = 0.0f;  //[0, 1]
  float releaseMsMod = 0.0f;

  void reset();
  void updateParams();
  void setSampleRate(float newSampleRate);

  void start();
  void release();

  float nextSample();

  // ------ Getters ------ //
  float getCurrentLevel() const;
  EnvelopeStage getStage() const;
  bool isActive() const;
  bool isAttack() const;

 private:
  float sampleRate = 44100.0f;
  float level = 0.0f;

  EnvelopeStage currentStage = EnvelopeStage::OFF;

  float declickMs = 5.0f;

  float targetLevel = 0.0f;
  float stageIncrement = 0.0f;

  float attackTarget = 1.0f;

  float applyVel(float level) const;

  // Converts a level delta and duration to a single-sample increment.
  float calcCoef(float ms, float target);
  void attack();
};
