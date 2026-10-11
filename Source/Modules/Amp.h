
#pragma once

class Amp {
 public:
  // ------ Exposed Parameters ------ //
  float velocitySens;
  float volume;

  float velocity;

  float nextSample(float input, float envLevel);
  void earProtection();

 private:
};