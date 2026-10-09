#pragma once

// ------------------------====
// The single source of truth for every parameter.
//
// Column layout:
//   id          - C++ identifier, used for ParameterID::id and idParam members
//   display     - human-readable name shown in the DAW
//   min, max    - range bounds
//   step        - smallest increment (0.0 means "no forced step")
//   default     - initial value
//   centre      - value that sits at the slider midpoint (0 = linear, no skew)
//   unit        - unit label shown next to the value
//   dest        - path into SynthParams, e.g. osc1.semitone -> synth.p.osc1.semitone
//
// To add a parameter: append one X(...) line here, then add one field to the
// matching struct in SynthParams. Everything else expands automatically.
// ------------------------====

// id, display, min, max, step, default, centre, unit, dest

#define PARAM_FLOAT_LIST                                                                                 \
  /* ------Oscillators ------- */                                                                        \
  X(osc1Semitone, "Osc 1 Semitone", -48.0f, 48.0f, 1.0f, 0.0f, 0.0f, "semi", osc1.semitone)              \
  X(osc1Cents, "Osc 1 Cents", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "cent", osc1.cents)                     \
  X(osc1Shape, "Osc 1 Shape", -1.0f, 1.0f, 0.001f, 0.0f, 0.0f, "", osc1.shape)                           \
  X(osc1Volume, "Osc 1 Volume", 0.0f, 100.0f, 1.0f, 100.0f, 0.0f, "%", osc1.volume)                      \
  X(osc2Semitone, "Osc 2 Semitone", -48.0f, 48.0f, 1.0f, 0.0f, 0.0f, "semi", osc2.semitone)              \
  X(osc2Cents, "Osc 2 Cents", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "cent", osc2.cents)                     \
  X(osc2Shape, "Osc 2 Shape", -1.0f, 1.0f, 0.001f, 0.0f, 0.0f, "", osc2.shape)                           \
  X(osc2Volume, "Osc 2 Volume", 0.0f, 100.0f, 1.0f, 100.0f, 0.0f, "%", osc2.volume)                      \
                                                                                                         \
  /* -------------------- Filter -------------------- */                                                 \
  X(filterAttack, "Filter Attack", 0.1f, 10.0f, 0.001f, 0.1f, 1.0f, "s", filter.attack)                  \
  X(filterDecay, "Filter Decay", 0.1f, 10.0f, 0.001f, 1.0f, 1.0f, "s", filter.decay)                     \
  X(filterSustain, "Filter Sustain", 0.1f, 10.0f, 0.001f, 5.0f, 1.0f, "", filter.sustain)                \
  X(filterRelease, "Filter Release", 0.1f, 10.0f, 0.001f, 1.0f, 1.0f, "s", filter.release)               \
  X(filterCutoff, "Filter Cutoff", 20.0f, 20000.0f, 0.1f, 20000.0f, 1000.0f, "Hz", filter.cutoff)        \
  X(filterResonance, "Filter Resonance", 0.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", filter.resonance)          \
  X(filterKeytrack, "Filter Keytrack", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", filter.keytrack)          \
  X(filterEnvAmount, "Filter Env Amount", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", filter.envAmount)      \
                                                                                                         \
  /* -------------------- Amp -------------------- */                                                    \
  X(ampAttack, "Amp Attack", 0.1f, 10.0f, 0.001f, 0.01f, 1.0f, "s", amp.attack)                          \
  X(ampDecay, "Amp Decay", 0.1f, 10.0f, 0.001f, 0.5f, 1.0f, "s", amp.decay)                              \
  X(ampSustain, "Amp Sustain", 0.1f, 10.0f, 0.001f, 5.0f, 1.0f, "", amp.sustain)                         \
  X(ampRelease, "Amp Release", 0.1f, 10.0f, 0.001f, 0.5f, 1.0f, "s", amp.release)                        \
  X(ampVelocity, "Amp Velocity", 0.0f, 100.0f, 1.0f, 50.0f, 0.0f, "%", amp.velocity)                     \
  X(ampVolume, "Amp Volume", 0.0f, 100.0f, 1.0f, 80.0f, 0.0f, "%", amp.volume)                           \
                                                                                                         \
  /* -------------------- LFO 1 -------------------- */                                                  \
  X(lfo1Rate, "LFO 1 Rate", 0.01f, 40.0f, 0.001f, 1.0f, 1.0f, "Hz", lfo1.rate)                           \
  X(lfo1Amplitude, "LFO 1 Amplitude", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", lfo1.amplitude)            \
  X(lfo1CutoffFreq, "LFO 1 Cutoff Freq", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", lfo1.cutoffFreq)        \
  X(lfo1VoiceFreq, "LFO 1 Voice Freq", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", lfo1.voiceFreq)           \
  X(lfo1Shape1, "LFO 1 Shape 1", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", lfo1.shape1)                    \
  X(lfo1Shape2, "LFO 1 Shape 2", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", lfo1.shape2)                    \
                                                                                                         \
  /* -------------------- LFO 2 -------------------- */                                                  \
  X(lfo2Rate, "LFO 2 Rate", 0.01f, 40.0f, 0.001f, 1.0f, 1.0f, "Hz", lfo2.rate)                           \
  X(lfo2Amplitude, "LFO 2 Amplitude", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", lfo2.amplitude)            \
  X(lfo2CutoffFreq, "LFO 2 Cutoff Freq", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", lfo2.cutoffFreq)        \
  X(lfo2VoiceFreq, "LFO 2 Voice Freq", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", lfo2.voiceFreq)           \
  X(lfo2Shape1, "LFO 2 Shape 1", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", lfo2.shape1)                    \
  X(lfo2Shape2, "LFO 2 Shape 2", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", lfo2.shape2)                    \
                                                                                                         \
  /* -------------------- Mod Envelope -------------------- */                                           \
  X(modEnvAttack, "Mod Env Attack", 0.1f, 10.0f, 0.001f, 0.01f, 1.0f, "s", modEnv.attack)                \
  X(modEnvDecay, "Mod Env Decay", 0.1f, 10.0f, 0.001f, 0.5f, 1.0f, "s", modEnv.decay)                    \
  X(modEnvSustain, "Mod Env Sustain", 0.1f, 10.0f, 0.001f, 5.0f, 1.0f, "", modEnv.sustain)               \
  X(modEnvRelease, "Mod Env Release", 0.1f, 10.0f, 0.001f, 0.5f, 1.0f, "s", modEnv.release)              \
  X(modEnvOsc2Volume, "Mod Env Osc 2 Volume", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", modEnv.osc2Volume) \
  X(modEnvVoiceFreq, "Mod Env Voice Freq", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", modEnv.voiceFreq)     \
  X(modEnvShape1, "Mod Env Shape 1", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", modEnv.shape1)              \
  X(modEnvShape2, "Mod Env Shape 2", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", modEnv.shape2)              \
                                                                                                         \
  /* -------------------- Velocity -------------------- */                                               \
  X(velShape1, "Vel Shape 1", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", vel.shape1)                        \
  X(velShape2, "Vel Shape 2", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", vel.shape2)                        \
  X(velCutoff, "Vel Cutoff", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", vel.cutoff)                         \
  X(velResonance, "Vel Resonance", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", vel.resonance)                \
  X(velAttack, "Vel Attack", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", vel.attack)                         \
  X(velDecay, "Vel Decay", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", vel.decay)                            \
  X(velSustain, "Vel Sustain", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", vel.sustain)                      \
  X(velRelease, "Vel Release", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", vel.release)                      \
                                                                                                         \
  /* -------------------- Global -------------------- */                                                 \
  X(globalGlide, "Glide", 0.0f, 10.0f, 0.1f, 0.0f, 1.0f, "s", global.glide)                              \
  X(globalTuning, "Global Tuning", 400.0f, 480.0f, 0.1f, 440.0f, 440.0f, "Hz", global.tuning)

// Choice parameters: no range, has a list of names.
// Columns: id, display, names (pipe-separated), default index, dest
#define PARAM_CHOICE_LIST                                                     \
  X(lfo1Shape, "LFO 1 Shape", "Sine|Tri|Saw|Square|Thin Rect", 0, lfo1.shape) \
  X(lfo2Shape, "LFO 2 Shape", "Sine|Tri|Saw|Square|Thin Rect", 0, lfo2.shape)

// Bool parameters: no range, no unit.
// Columns: id, display, default, dest
#define PARAM_BOOL_LIST                                                   \
  X(ampAttackFromZero, "Amp Attack From Zero", false, amp.attackFromZero) \
  X(lfo1Keytrig, "LFO 1 Keytrig", false, lfo1.keytrig)                    \
  X(lfo2Keytrig, "LFO 2 Keytrig", false, lfo2.keytrig)

// Int parameters: min/max integer, step is implied as 1.
// Columns: id, display, min, max, default, dest
#define PARAM_INT_LIST \
  X(globalVoiceCount, "Voice Count", 1, 8, 8, global.voiceCount)