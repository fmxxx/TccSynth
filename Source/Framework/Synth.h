#pragma once

#include <JuceHeader.h>

#include <cstdint>

#include "SynthParams.h"
#include "Voice.h"

constexpr int MAX_VOICES = 8;
constexpr float POLY_ATTENUATION = 1 / MAX_VOICES;
constexpr int CONTROL_RATE_SAMPLES = 32;

class Synth {
 public:
  void allocateResources(double sampleRateNew, int samplesPerBlock);
  void deallocateResources();

  void reset();
  void updateVoicesParams();
  void render(float** outputBuffers, int sampleCount);
  void receiveMidi(uint8_t data0, uint8_t data1, uint8_t data2);

  SynthParams p;

 private:
  Voice voices[MAX_VOICES];

  float sampleRate;

  int controlCounter = 1;  // Keeps track of when the LFO should update

  void
  controlChange(uint8_t data1, uint8_t data2);
  void noteOn(float note, float velocity);
  void noteOff(float note);
  void startVoices(int voices, float note, float velocity);
  void restartMonoVoice(float mideNote, float velocity);

  int findNewVoice(float note) const;
  void shiftQueuedNotes();
  int nextQueuedNote();
  bool isPlayingLegatoStyle() const;

  // Reset unused voices when polyphony changes and update its params
  void setPolyphony(int newVoiceCount);
};