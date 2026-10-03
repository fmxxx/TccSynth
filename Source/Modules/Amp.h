
#pragma once

class Amp {
 public:
  // ------ Exposed Parameters ------ //
  float velocitySens;
  float volume;

  float velocity;

  void updateParams();
  float nextSample(float input);
  void earProtection();

 private:
};