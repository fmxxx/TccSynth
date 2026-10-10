
#pragma once

class Filter {
 public:
  // ------ Exposed Parameters ------ //
  float cutoffFreq;
  float resonance;  // [0, 1], 0 = no resonance, 1 = self-oscillation
  float keytrack;   // [-1, 1]
  float envAmt;     // [-1, 1]

  // ------ Modulations (block-rate) ------ //
  float cutoffFreqModAmt;  // [-1, 1]
  float resonanceModAmt;   // [-1, 1]

  void reset();
  void updateParams();
  void setsampleRate(float newsampleRate);
  float nextSample(float input);

 private:
  float sampleRate = 44100.0f;

  // ------ Precomputed coefficients (calculated in updateParams) ------ //
  float g;   // tan(pi * fc / fs) — the ZDF frequency warp
  float G;   // g / (1 + g)       — the ZDF integrator gain
  float G4;  // G^4               — for the feedback solve
  float k;   // resonance feedback coefficient, [0, 4]

  // Coefficients for the state summation in the feedback solve
  float c1;  // G^3 * (1 - G)
  float c2;  // G^2 * (1 - G)
  float c3;  // G   * (1 - G)
  float c4;  // (1 - G)

  float oneOverOnePlusKG4;  // 1 / (1 + k * G^4)

  // ------ Integrator states (one per pole) ------ //
  float s1 = 0.0f;
  float s2 = 0.0f;
  float s3 = 0.0f;
  float s4 = 0.0f;
};