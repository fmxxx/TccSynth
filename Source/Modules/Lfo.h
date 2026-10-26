
#pragma once

class Lfo {
 public:
  // ------ Exposed Parameters ------ //
  float freq = 1.0f;
  bool keytrig = false;
  int shape = 1;
  float amplitude = 1.0f;  // [0,1]

  void reset();
  void setPhase(float newPhase);

  void updateParams();
  void setSampleRate(float newSampleRate);

  float nextSample();

 private:
  float sampleRate = 44100.0f;

  float phase = 0.0f;     // [0, 1]
  float phaseInc = 0.0f;  // Phase increment
};