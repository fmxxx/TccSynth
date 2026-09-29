#pragma once

class Oscillator {
 public:
  // ------ Exposed Parameters ------ //
  float shape;      // -1: tri; 0: saw; 0.5: square; 1: narrow pulse
  float amplitude;  // [0,1]

  // ------ Modulations ------ //
  float shapeMod;  // [-1 ,1]
  float volMod;    // [-1 ,1]

  float midinote;
  float period = 0.0f;

  void updateParams();
  void reset();
  void setsampleRate(float newsampleRate);
  float poly_blep(float phase, float phaseInc);
  float nextSample();

 private:
  float sampleRate;

  const float phaseMax = 1.0f;  // Phase goes up to this value
  float phase;                  // [0, 1]
  float phaseInc;               // Phase increment
};