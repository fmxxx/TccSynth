
#include "Envelope.h"

void Envelope::reset() {
  currentStage = EnvelopeStage::OFF;
  level = 0.0f;
  targetLevel = 0.0f;
  stageIncrement = 0.0f;
}

void Envelope::updateParams() {}

void Envelope::setSampleRate(float newSampleRate) {
  sampleRate = newSampleRate;
}

// Called "start" not "attack" because I might add delay phase
void Envelope::start() {
  // On retrigger, ramp the current level to zero before starting the attack
  if (attackFromZero && level > 0.0f && declickMs > 0.0f) {
    currentStage = EnvelopeStage::DECLICK;
    targetLevel = 0.0f;
    stageIncrement = calcCoef(declickMs, -level);
    return;
  }

  if (attackFromZero) {
    level = 0.0f;
  }

  attack();
}

void Envelope::attack() {
  // A zero-length attack or decay completes immediately
  if (attackMs <= 0.0f) {
    level = attackTarget;
    targetLevel = std::clamp(sustain, 0.0f, 1.0f);
    if (decayMs <= 0.0f) {
      level = targetLevel;
      currentStage = EnvelopeStage::SUSTAIN;
      stageIncrement = 0.0f;
    } else {
      currentStage = EnvelopeStage::DECAY;
      stageIncrement = calcCoef(decayMs, targetLevel - level);
    }
    return;
  }

  currentStage = EnvelopeStage::ATTACK;
  targetLevel = attackTarget;
  stageIncrement = calcCoef(attackMs, targetLevel - level);
}

void Envelope::release() {
  if (currentStage == EnvelopeStage::OFF) {
    return;
  }

  // Release begins at the current level, including if the note is still in
  // its attack or declick stage
  if (releaseMs <= 0.0f) {
    level = 0.0f;
    currentStage = EnvelopeStage::OFF;
    stageIncrement = 0.0f;
    return;
  }

  currentStage = EnvelopeStage::RELEASE;
  targetLevel = 0.0f;
  stageIncrement = calcCoef(releaseMs, targetLevel - level);
}

float Envelope::applyVel(float level) const {
  return level * (velocityNorm * velocitySens + (1 - velocitySens));
}

float Envelope::nextSample() {
  // Advance the current segment by one sample and
  // switch stages when it hits  its target
  switch (currentStage) {
    case EnvelopeStage::OFF:
      level = 0.0f;
      return applyVel(level);

    case EnvelopeStage::DECLICK:
      level += stageIncrement;
      if (level <= targetLevel) {
        level = targetLevel;
        attack();
      }
      return applyVel(level);

    case EnvelopeStage::ATTACK:
      if (attackMs <= 0.0f) {
        level = targetLevel;
      } else {
        level += stageIncrement;
        if ((stageIncrement >= 0.0f && level >= targetLevel) ||
            (stageIncrement < 0.0f && level <= targetLevel)) {
          level = targetLevel;
        }
      }
      if (level == targetLevel) {
        targetLevel = std::clamp(sustain, 0.0f, 1.0f);
        if (decayMs <= 0.0f) {
          level = targetLevel;
          currentStage = EnvelopeStage::SUSTAIN;
          stageIncrement = 0.0f;
        } else {
          currentStage = EnvelopeStage::DECAY;
          stageIncrement = calcCoef(decayMs, targetLevel - level);
        }
      }
      return applyVel(level);

    case EnvelopeStage::DECAY:
      if (decayMs <= 0.0f) {
        level = targetLevel;
      } else {
        level += stageIncrement;
        if ((stageIncrement >= 0.0f && level >= targetLevel) ||
            (stageIncrement < 0.0f && level <= targetLevel)) {
          level = targetLevel;
        }
      }
      if (level == targetLevel) {
        currentStage = EnvelopeStage::SUSTAIN;
        stageIncrement = 0.0f;
      }
      return applyVel(level);

    case EnvelopeStage::SUSTAIN:
      level = std::clamp(sustain, 0.0f, 1.0f);
      return applyVel(level);

    case EnvelopeStage::RELEASE:
      if (releaseMs <= 0.0f) {
        level = 0.0f;
      } else {
        level += stageIncrement;
        if (level <= targetLevel) {
          level = targetLevel;
        }
      }
      if (level <= targetLevel) {
        currentStage = EnvelopeStage::OFF;
        stageIncrement = 0.0f;
      }
      return applyVel(level);
  }

  return applyVel(level);
}

float Envelope::getCurrentLevel() const {
  return applyVel(level);
}

EnvelopeStage Envelope::getStage() const {
  return currentStage;
}

bool Envelope::isActive() const {
  return currentStage != EnvelopeStage::OFF;
}

bool Envelope::isAttack() const {
  return currentStage == EnvelopeStage::ATTACK;
}

float Envelope::calcCoef(float timeMs, float target) {
  if (timeMs <= 0.0f || sampleRate <= 0.0f) {
    return target;
  }
  return target / (0.001f * timeMs * sampleRate);
}