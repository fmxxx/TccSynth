
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
  void updateParams();

  void start();
  void release();

  float nextSample();

  // ------ Setters ------ //
  void setSampleRate(float newSampleRate);

  // ------ Getters ------ //
  float getCurrentLevel() const;
  float getCurrentStageAsFloat() const;
  bool isActive() const;
  bool isAttack() const;

 private:
  float sampleRate = 44100.0f;

  float level = 0.0f;
  float increment = 0.0f;
  EnvStage currentStage = EnvStage::OFF;

  const float attackTarget = 1.0f;
  const float releaseTarget = 0.0f;

  float applyVel(float level) const;

  // Converts a level delta and duration to a single-sample increment.
  float calcInc(float ms, float targetLevel, float currentLevel);
  void attack();
};
