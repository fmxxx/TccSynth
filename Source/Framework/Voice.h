#pragma once

#include <cstdint>

#include "../Modules/Envelope.h"
#include "../Modules/Filter.h"
#include "../Modules/Oscillator.h"

class Voice {
 public:
  float note;

  float period;

  float target;

  Oscillator osc1;
  Oscillator osc2;

  Envelope envAmp;
  Envelope envFilter;
  Envelope envMod;

  void reset();
  void updateModules();
  void updateLfos();
  float render(float input);
  void release();

 private:
  // Returns period in samples
  float periodFromNote(float sampleRate, float globalTuning, float semitone, float cents);
  // Returns freq in Hz
  float freqFromNote(float sampleRate, float globalTuning, float semitone, float cents);
  // Updates every 32 samples
};
