#include "Synth.h"

void Synth::allocateResources(double sampleRateNew, int /*samplesPerBlock*/) {
  sampleRate = static_cast<float>(sampleRateNew);
  for (int v = 0; v < p.global.voiceCount; ++v) {
    voices[v].sampleRate = sampleRate;
  }

  controlCounter = 1;
}

void Synth::deallocateResources() {}

void Synth::reset() {
  for (int v = 0; v < MAX_VOICES; v++) {
    voices[v].reset();
  }
};

void Synth::render(float** outputBuffers, int sampleCount) {
  float* left = outputBuffers[0];
  float* right = outputBuffers[1];

  // --- Update modulators every CONTROL_RATE_SAMPLES samples ---
  for (int i = 0; i < sampleCount; ++i) {
    if (--controlCounter <= 0) {
      controlCounter = CONTROL_RATE_SAMPLES;
      for (int v = 0; v < p.global.voiceCount; ++v) {
        if (voices[v].envAmp.isActive()) {
          voices[v].updateSlowMods();
        }
      }
    }

    // --- Render all active voices for this sample ---
    float sample = 0.0f;

    for (int v = 0; v < p.global.voiceCount; ++v) {
      if (voices[v].envAmp.isActive()) {
        sample += voices[v].render() * POLY_ATTENUATION;  // Reduces volume so multiple voices dont make things too loud!
      }
    }

    // --- Write to output ---
    left[i] = sample;
    if (right != nullptr) {
      right[i] = sample;
    }
  }
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

void Synth::noteOn(float note, float velocity) {
  int voice = 0;  // index of the voice to use (0 = mono voice)

  if (p.global.voiceCount == 1) {  // monophonic
    if (voices[0].note > 0) {      // legato-style playing
      shiftQueuedNotes();
      restartMonoVoice(note, velocity);
      return;
    }
  } else {  // polyphonic
    voice = findNewVoice(note);
  }

  startVoices(voice, note, velocity);
}

void Synth::noteOff(float note) {
  // In mono mode and the current playing note is release
  if ((p.global.voiceCount == 1) && (voices[0].note == note)) {
    // Find if an older note is still held down
    int queuedNote = nextQueuedNote();
    if (queuedNote > 0) {
      // Set the older note to 0 and restart it
      restartMonoVoice(queuedNote, -1);
    }
  }

  // In poly mode or if there's no other key held in mono mode
  for (int v = 0; v < p.global.voiceCount; ++v) {
    // Any voices playing this note?
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
  // 2. Non active voice
  // 3. Quietest voice
  // 4. 0th Voice

  int v = 0;  // The free voice

  // Same note voice
  for (int i = 0; i < p.global.voiceCount; ++i) {
    if (voices[i].note == note) {
      v = i;
      return v;
    }
  }

  // Inactive voice
  for (int i = 0; i < p.global.voiceCount; ++i) {
    if (!voices[i].envAmp.isActive()) {
      v = i;
      return v;
    }
  }

  // Quietest non attack voice
  float l = 100.0f;

  for (int i = 0; i < p.global.voiceCount; ++i) {
    if (voices[i].envAmp.getCurrentValue() < l && !voices[i].envAmp.isAttack()) {
      l = voices[i].envAmp.getCurrentValue();
      v = i;
    }
  }

  return v;
}

void Synth::startVoices(int v, float note, float velocity) {
  // TODO: This function will handle legato and glide (by calling the setGlideTarget() function and storting the previous note for trills

  voices[v].start(note, velocity);
}

void Synth::updateVoicesParams() {
  for (int v = 0; v < p.global.voiceCount; ++v) {
    voices[v].updateParams(p);
  }
};

void Synth::setPolyphony(int newVoiceCount) {}