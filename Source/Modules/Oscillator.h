#pragma once

class Oscillator {
 public:
  Oscillator();

  // ------ Exposed Parameters ------ //
  float shape = 0.0f;      // -1: tri; 0: saw; 0.5: square; 1: narrow pulse
  float amplitude = 0.0f;  // [0, 1]

  // Phase increment, normalized phase per sample (cycles/sample).
  //   phaseInc = frequency / sampleRate
  //   phaseInc = 1.0f / samplesPerCycle
  float phaseInc;

  void applyModulations(float shapeMod, float volMod);  // Called by Voice

  void reset();
  void newNote();

  float poly_blep(float phase, float phaseInc);
  float nextSample();

 private:
  float phase;  // [0, 1)

  // Internal helpers
  float saw(float p, float dt);
  float triangleShape(float v1, float slant);
};