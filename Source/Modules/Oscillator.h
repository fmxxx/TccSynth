#pragma once

class Oscillator {
 public:
  // ------ Exposed Parameters ------ //
  float shape = 0.0f;      // -1: tri; 0: saw; 0.5: square; 1: narrow pulse
  float amplitude = 1.0f;  // [0, 1]

  // Phase increment, normalized phase per sample (cycles/sample).
  //   phaseInc = frequency / sampleRate
  //   phaseInc = 1.0f / samplesPerCycle
  float phaseInc = 0.0f;

  void setPhase(float newPhase);

  // ------ DSP ------ //
  float poly_blep(float phase, float phaseInc);
  float nextSample();

 private:
  float phase = 0.0;  // [0, 1)

  // Internal helpers
  float saw(float p, float dt);
  float triangleShape(float v1, float slant);
};