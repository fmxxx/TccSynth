#pragma once

#include <cstdint>

#include "Voice.h"

struct SynthParams {
  struct {
    float semitone, cents, shape, volume;
  } osc1, osc2;

  struct {
    float attack, decay, sustain, release;
    float cutoff, resonance, keytrack, envAmount;
  } filter;

  struct {
    float attack, decay, sustain, release;
    bool attackFromZero;
    float velocity, volume;
  } amp;

  struct {
    float rate;
    int shape;
    bool sync;
    float amplitude, cutoffFreq, voiceFreq, shape1, shape2;
  } lfo1, lfo2;

  struct {
    float attack, decay, sustain, release;
    float osc2Volume, voiceFreq, shape1, shape2;
  } modEnv;

  struct {
    float shape1, shape2, cutoff, resonance;
    float attack, decay, sustain, release;
  } vel;

  struct {
    int voiceCount;
    float glide;
    float tuning;
  } global;
};

constexpr int MAX_VOICES = 8;
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

  void controlChange(uint8_t data1, uint8_t data2);
  void noteOn(int note, int velocity);
  void noteOff(int note);
  void startVoice(int voices, int note, int velocity);
  void restartMonoVoice(int note, int velocity);

  int findFreeVoice() const;
  void shiftQueuedNotes();
  int nextQueuedNote();
  bool isPlayingLegatoStyle() const;
};