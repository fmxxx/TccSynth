#include "Synth.h"

void Synth::reset() {
  for (int v = 0; v < MAX_VOICES; v++) {
    voices[v].reset();
  }
};

void Synth::updateVoices() {
  // TODO update global parameters

  for (int v = 0; v < MAX_VOICES; ++v) {
    voices[v].updateParams(p);
  }
}

void Synth::render(float** outputBuffers, int sampleCount) {
  float* left = outputBuffers[0];
  float* right = outputBuffers[1];

  // --- Update modulators every LFO_UPDATE_SAMPLES samples ---
  for (int i = 0; i < sampleCount; ++i) {
    if (--lfoCounter <= 0) {
      lfoCounter = LFO_UPDATE_SAMPLES;
      for (int v = 0; v < MAX_VOICES; ++v) {
        if (voices[v].isActive) {
          voices[v].updateLfos();
        }
      }
    }

    // --- Render all active voices for this sample ---
    float sample = 0.0f;

    for (int v = 0; v < MAX_VOICES; ++v) {
      if (voices[v].isActive) {
        sample += voices[v].render() * POLY_ATTENUATION;
      }
    }

    // --- Write to output ---
    left[i] = sample;
    if (right != nullptr) {
      right[i] = sample;
    }
  }
}
