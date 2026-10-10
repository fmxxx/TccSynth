
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
  void setSampleRate(float newSampleRate);

  float nextSample();

 private:
  float sampleRate = 44100.0f;

  const float phaseMax = 1.0f;  // Phase goes up to this value
  float phase = 0.0f;           // [0, 1]
  float phaseInc = 0.0f;        // Phase increment

  float getPeriodFromFreq(float freq, float sampleRate);
};