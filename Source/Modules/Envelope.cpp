#include "Envelope.h"

void Envelope::reset() {
  level = 0.0f;
  increment = 0.0f;
  currentStage = EnvStage::OFF;
}

void Envelope::updateParams() {
  // Blank for now, until we add feature to udpate coefs in real time
}

void Envelope::start() {
  if (attackFromZero) {
    level = 0.0f;
  }
  currentStage = EnvStage::ATTACK;
  increment = calcInc(attackMs, attackTarget, level);  // Increment for the next stage
}

void Envelope::release() {
  if (currentStage == EnvStage::OFF || currentStage == EnvStage::RELEASE) {
    return;  // Do nothing if its off or already in release stage
  }
  currentStage = EnvStage::RELEASE;
  increment = calcInc(releaseMs, releaseTarget, level);  // Increment for the next stage
}

float Envelope::nextSample() {
  switch (currentStage) {
    case EnvStage::ATTACK:

      if (level >= attackTarget) {
        currentStage = EnvStage::DECAY;
        increment = calcInc(decayMs, sustain, level);  // Increment for the next stage
      } else {
        level += increment;
        return applyVel(level);
      }
    case EnvStage::DECAY:
      if (level <= sustain) {
        currentStage = EnvStage::SUSTAIN;
      } else {
        level += increment;
        return applyVel(level);
      }

    case EnvStage::SUSTAIN:
      // Increment for the next stage calculated in release();
      return applyVel(level);

    case EnvStage::RELEASE:
      if (level <= releaseTarget) {
        currentStage = EnvStage::OFF;
      } else {
        level += increment;
        return applyVel(level);
      }

    case EnvStage::OFF:
      level = 0.0f;
      return 0.0f;

    default:
      break;
  }
}

float Envelope::calcInc(float timeMs, float currentLevel, float targetLevel) {
  float delta = currentLevel - targetLevel;
  if (timeMs <= 0.0f) {
    return delta;
  }
  return delta / (0.001f * timeMs * sampleRate);
}

float Envelope::applyVel(float level) const {
  float levelScaled = level * (velocityNorm * velocitySens + (1 - velocitySens));
  return std::clamp(levelScaled, 0.0f, 1.0f);
}

// ------ Setters ------ //
void Envelope::setSampleRate(float newSampleRate) {
  sampleRate = newSampleRate;
}

// ------ Getters ------ //
float Envelope::getCurrentLevel() const {
  return level;
}

bool Envelope::isActive() const {
  return (currentStage != EnvStage::OFF);
}

bool Envelope::isAttack() const {
  return (currentStage == EnvStage::ATTACK);
}