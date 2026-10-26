
#pragma once

class Filter {
 public:
  // ------ Exposed Parameters ------ //
  float cutoffFreq = 20000.0f;
  float resonance = 0.0f;  // [0, 1], 0 = no resonance, 1 = self-oscillation

  Filter();
  void reset();
  void setsampleRate(float newsampleRate);

  void updateParams();
  void applyModulations(float cutoffMod, float resonanceMod);

  float nextSample(float input);

 private:
  float sampleRate = 44100.0f;

  // ------ Precomputed coefficients (calculated in updateParams) ------ //
  float g = 0.0f;   // tan(pi * fc / fs) — the ZDF frequency warp
  float G = 0.0f;   // g / (1 + g)       — the ZDF integrator gain
  float G4 = 0.0f;  // G^4               — for the feedback solve
  float k = 0.0f;   // resonance feedback coefficient, [0, 4]

  // Coefficients for the state summation in the feedback solve
  float c1 = 0.0f;  // G^3 * (1 - G)
  float c2 = 0.0f;  // G^2 * (1 - G)
  float c3 = 0.0f;  // G   * (1 - G)
  float c4 = 0.0f;  // (1 - G)

  float oneOverOnePlusKG4 = 1.0f;  // 1 / (1 + k * G^4)

  // ------ Integrator states (one per pole) ------ //
  float s1 = 0.0f;
  float s2 = 0.0f;
  float s3 = 0.0f;
  float s4 = 0.0f;

  void calcCoefs();
};