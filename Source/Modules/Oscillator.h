#pragma once

class Oscillator {
 public:
  // ------ Exposed Parameters ------ //
  float shape;      // -1: tri; 0: saw; 0.5: square; 1: narrow pulse
  float amplitude;  // [0,1]

  // ------ Modulations ------ //
  float shapeMod;  // [-1 ,1]
  float volMod;    // [-1 ,1]

  float phaseInc;  // Phase increment

  void updateParams();
  void reset();
  void newNote();
  float poly_blep(float phase, float phaseInc);
  float nextSample();

 private:
  float sampleRate;

  const float phaseMax = 1.0f;  // Phase goes up to this value
  float phase;                  // [0, 1]
};