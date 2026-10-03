#pragma once

#include <cstdint>

#include "SynthParams.h"
#include "Voice.h"

constexpr int MAX_VOICES = 8;
constexpr float POLY_ATTENUATION = 1 / MAX_VOICES;
constexpr int LFO_UPDATE_SAMPLES = 32;

class Synth {
 public:
  void allocateResources(float sampleRateNew, int samplesPerBlock);
  void deallocateResources();

  void reset();
  void updateVoices();  // Calls every voices' update()
  void render(float** outputBuffers, int sampleCount);
  void receiveMidi(uint8_t data0, uint8_t data1, uint8_t data2);

  SynthParams p;

 private:
  Voice voices[MAX_VOICES];

  float sampleRate;

  int lfoCounter = 1;  // Keeps track of when the LFO should update

  void
  controlChange(uint8_t data1, uint8_t data2);
  void noteOn(int note, int velocity);
  void noteOff(int note);
  void startVoice(int voices, int note, int velocity);
  void restartMonoVoice(int note, int velocity);

  int findFreeVoice() const;
  void shiftQueuedNotes();
  int nextQueuedNote();
  bool isPlayingLegatoStyle() const;
};