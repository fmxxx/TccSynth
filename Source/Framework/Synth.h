#pragma once

#include <JuceHeader.h>

#include <cstdint>

#include "SynthParams.h"
#include "Voice.h"

constexpr int MAX_VOICES = 8;
constexpr float POLY_ATTENUATION = 1.0f / static_cast<float>(MAX_VOICES);
constexpr int CONTROL_RATE_SAMPLES = 32;

void protectYourEars(float* buffer, int sampleCount);

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
  int polyphony = 8;
  Voice voices[MAX_VOICES];

  int controlCounter = 1;  // Keeps track of when the LFO should update

  void controlChange(uint8_t data1, uint8_t data2);
  void noteOn(float note, float velocity);
  void noteOff(float note);

  int findNewVoice(float note) const;

  // Reset unused voices when polyphony changes and update its params
  void setPolyphony(int newVoiceCount);
};