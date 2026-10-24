
#pragma once

class Amp {
 public:
  // ------ Exposed Parameters ------ //
  float velocitySens = 0.0f;  // [0,1]
  float volume = 0.0f;        // [0,1]

  float velocityNorm = 0.0f;  // [0,1]

  float output = 0.0f;

  float nextSample(float input, float envLevel);

 private:
};