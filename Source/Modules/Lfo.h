
#pragma once

class Lfo {
 public:
  // ------ Exposed Parameters ------ //
  float freq;
  bool keytrig;
  int shape;
  float amplitude;  // [0,1]

  float period = 0.0f;

  void reset();
  void updateParams();
  float nextSample();

 private:
  float sampleRate;

  const float phaseMax = 1.0f;  // Phase goes up to this value
  float phase;                  // [0, 1]
  float phaseInc;               // Phase increment

  float getPeriodFromFreq(float freq, float sampleRate);
};