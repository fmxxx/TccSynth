#include "Synth.h"

#include <cmath>
#include <cstddef>
#include <cstring>

#include "../Constants.h"

void Synth::allocateResources(double sampleRateNew, int /*samplesPerBlock*/) {
  const float sampleRate = static_cast<float>(sampleRateNew);
  // Sets sampleRate, initializes and updates parameters
  // Of every voice (even unused)
  for (int v = 0; v < MAX_VOICES; ++v) {
    voices[v].setSampleRate(sampleRate);
    voices[v].reset();
    voices[v].importParams(p);
    voices[v].updateParams();
    voices[v].randomizeLfoPhase(v);
  }

  controlCounter = 1;
}

void Synth::deallocateResources() {}

void Synth::reset() {
  for (int v = 0; v < MAX_VOICES; v++) {
    voices[v].reset();
  }
};

void Synth::updateVoicesParams() {
  if (p.global.voiceCount != polyphony) {
    setPolyphony(p.global.voiceCount);
  }
  for (int v = 0; v < polyphony; ++v) {
    voices[v].importParams(p);
    voices[v].updateParams();
  }
};

void Synth::render(float** outputBuffers, int sampleCount) {
  float* left = outputBuffers[0];
  float* right = outputBuffers[1];

  // --- Update modulators every CONTROL_RATE_SAMPLES samples ---
  for (int i = 0; i < sampleCount; ++i) {
    if (--controlCounter <= 0) {
      controlCounter = CONTROL_RATE_SAMPLES;
      for (int v = 0; v < polyphony; ++v) {
        // Do even in inactive voices
        voices[v].advanceLfos();
        voices[v].advanceModEnvs();
        // Do only in active voices
        if (voices[v].isActive()) {
          voices[v].updateSlowMods();  // update envs and mod amounts
        }
      }
    }

    // --- Render all active voices for this sample ---
    float sample = 0.0f;

    for (int v = 0; v < polyphony; ++v) {
      if (voices[v].isActive()) {
        sample += voices[v].render() * POLY_ATTENUATION;  // Reduces volume so multiple voices dont make things too loud!
      }
    }

    // --- Write to output ---
    left[i] = sample;
    if (right != nullptr) {
      right[i] = sample;
    }
  }

  protectYourEars(left, sampleCount);
  protectYourEars(right, sampleCount);
}

void Synth::receiveMidi(uint8_t data0, uint8_t data1, uint8_t data2) {
  switch (data0 & 0xF0) {  // status byte (all channels)
    // Note off
    case 0x80:
      noteOff(data1 & 0x7F);
      break;

    // Note on
    case 0x90: {
      uint8_t note = data1 & 0x7F;
      uint8_t velocity = data2 & 0x7F;
      if (velocity > 0) {
        noteOn(note, velocity);
      } else {
        noteOff(note);
      }
      break;
    }

      // Control change
      /*
      case 0xB0:
        controlChange(data1, data2);
        break;
        */
  }
}

void Synth::controlChange(uint8_t data1, uint8_t data2) {}

void Synth::noteOn(float note, float velocity) {
  int voice = 0;  // index of the voice to use (0 = mono voice)

  if (polyphony > 1) {
    voice = findNewVoice(note);
  }

  voices[voice].startNote(note, velocity);
}

void Synth::noteOff(float note) {
  for (int v = 0; v < polyphony; ++v) {
    if (voices[v].note == note) {
      // TODO: Check for sustain pedal?
      voices[v].release();
      voices[v].note = 0;
    }
  }
}

int Synth::findNewVoice(float note) const {
  // Note priorities
  // 1. Same note as we're activating
  // 2. An inactive voice
  // 3. The voice with quietest envelope not in attack
  // 4. 0th Voice

  int v = 0;  // The free voice

  // Same note as we're activating
  for (int i = 0; i < polyphony; ++i) {
    if (voices[i].note == note) {
      v = i;
      return v;
    }
  }

  // An inactive voice
  for (int i = 0; i < polyphony; ++i) {
    if (!voices[i].isActive()) {
      v = i;
      return v;
    }
  }

  // The voice with quietest envelope not in attack
  float l = 100.0f;  // Large value (louder than any sample)

  for (int i = 0; i < polyphony; ++i) {
    if (!voices[i].envAmp.isAttack()) {
      const float level = voices[i].envAmp.getCurrentLevel();
      if (level < l) {
        l = level;
        v = i;
      }
    }
  }
  return v;
}

void Synth::setPolyphony(int newVoiceCount) {
  // Resets voices that will be unused
  for (int v = newVoiceCount; v < MAX_VOICES; ++v) {
    voices[v].reset();
    // TODO: Implement a "kill" method? Maybe unecessary since render uses
    // polyphony to choose what voices to render
  }
  polyphony = newVoiceCount;
}

void protectYourEars(float* buffer, int sampleCount) {
  if (buffer == nullptr) {
    return;
  }

  bool firstWarning = true;
  for (int i = 0; i < sampleCount; ++i) {
    const float x = buffer[i];
    bool silence = false;

    if (std::isnan(x)) {
      DBG("!!! WARNING: nan detected in audio buffer, silencing !!!");
      silence = true;
    } else if (std::isinf(x)) {
      DBG("!!! WARNING: inf detected in audio buffer, silencing !!!");
      silence = true;
    } else if (x < -2.0f || x > 2.0f) {
      DBG("!!! WARNING: sample out of range, silencing !!!");
      silence = true;
    } else if (x < -1.0f) {
      if (firstWarning) {
        DBG("!!! WARNING: sample out of range, clamping !!!");
        firstWarning = false;
      }
      buffer[i] = -1.0f;
    } else if (x > 1.0f) {
      if (firstWarning) {
        DBG("!!! WARNING: sample out of range, clamping !!!");
        firstWarning = false;
      }
      buffer[i] = 1.0f;
    }

    if (silence) {
      std::memset(buffer, 0, static_cast<std::size_t>(sampleCount) * sizeof(float));
      return;
    }
  }
}