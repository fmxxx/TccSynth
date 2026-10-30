
#pragma once

#include <algorithm>
#include <cmath>

enum class EnvStage {
  OFF,
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

  void reset();
  void updateParams();  // TODO: add coeficient recalculation on parameter change

  // ------ Lifecycle ------ //
  void start();
  void release();

  // ------ Per sample ------ //
  float nextSample();

  // ------ Setters ------ //
  void setSampleRate(float newSampleRate);

  // ------ Getters ------ //
  float getCurrentLevel() const;
  bool isActive() const;    // not OFF = True
  bool isAttack() const;    // ATTACK = True
  bool isReleased() const;  // RELEASE or OFF = True

 private:
  float sampleRate = 44100.0f;

  float level = 0.0f;
  float increment = 0.0f;
  EnvStage currentStage = EnvStage::OFF;

  const float attackTarget = 1.0f;
  const float releaseTarget = 0.0f;

  // ------ Helpers ------ //
  float applyVel(float level) const;
  // Converts a level delta and duration to a single-sample increment.
  float calcInc(float ms, float targetLevel, float currentLevel);
};
