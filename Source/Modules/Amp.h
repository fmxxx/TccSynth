
#pragma once

class Amp {
 public:
  float outputScale;
  float amplitude;

  void updateParams();
  float nextSample();
  void earProtection();

 private:
};