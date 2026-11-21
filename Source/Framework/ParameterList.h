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

#define PARAM_FLOAT_LIST                                                                                   \
  /* ------Oscillators ------- */                                                                          \
  X(osc1Semitone, "Osc 1 Semitone", -48.0f, 48.0f, 1.0f, 0.0f, 0.0f, "semi", osc1.semitones)               \
  X(osc1Cents, "Osc 1 Cents", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "cent", osc1.cents)                       \
  X(osc1Shape, "Osc 1 Shape", -1.0f, 1.0f, 0.0001f, 0.0f, 0.0f, "", osc1.shapeFctr)                        \
  X(osc1Volume, "Osc 1 Volume", 0.0f, 100.0f, 1.0f, 100.0f, 0.0f, "%", osc1.volPct)                        \
  X(osc2Semitone, "Osc 2 Semitone", -48.0f, 48.0f, 1.0f, 0.0f, 0.0f, "semi", osc2.semitones)               \
  X(osc2Cents, "Osc 2 Cents", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "cent", osc2.cents)                       \
  X(osc2Shape, "Osc 2 Shape", -1.0f, 1.0f, 0.0001f, 0.0f, 0.0f, "", osc2.shapeFctr)                        \
  X(osc2Volume, "Osc 2 Volume", 0.0f, 100.0f, 1.0f, 100.0f, 0.0f, "%", osc2.volPct)                        \
                                                                                                           \
  /* -------------------- Filter -------------------- */                                                   \
  X(filterAttack, "Filter Attack", 0.001f, 10.0f, 0.001f, 0.1f, 1.0f, "s", filter.attackSecs)              \
  X(filterDecay, "Filter Decay", 0.1f, 10.0f, 0.001f, 1.0f, 1.0f, "s", filter.decaySecs)                   \
  X(filterSustain, "Filter Sustain", 0.0f, 1.0f, 0.001f, 0.5f, 0.5f, "", filter.sustainFctr)               \
  X(filterRelease, "Filter Release", 0.1f, 10.0f, 0.001f, 1.0f, 1.0f, "s", filter.releaseSecs)             \
  X(filterCutoff, "Filter Cutoff", 20.0f, 20000.0f, 0.1f, 20000.0f, 1000.0f, "Hz", filter.cutoffHz)        \
  X(filterResonance, "Filter Resonance", 0.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", filter.resonancePct)         \
  X(filterKeytrack, "Filter Keytrack", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", filter.keytrackPct)         \
  X(filterEnvAmount, "Filter Env Amount", -20000.0f, 20000.0f, 1.0f, 0.0f, 0.0f, "Hz", filter.envAmountHz) \
                                                                                                           \
  /* -------------------- Amp -------------------- */                                                      \
  X(ampAttack, "Amp Attack", 0.001f, 10.0f, 0.001f, 0.1f, 1.0f, "s", amp.attackSecs)                       \
  X(ampDecay, "Amp Decay", 0.1f, 10.0f, 0.001f, 0.5f, 1.0f, "s", amp.decaySecs)                            \
  X(ampSustain, "Amp Sustain", 0.0f, 1.0f, 0.001f, 0.5f, 0.5f, "", amp.sustainFctr)                        \
  X(ampRelease, "Amp Release", 0.1f, 10.0f, 0.001f, 0.5f, 1.0f, "s", amp.releaseSecs)                      \
  X(ampVelocity, "Amp Velocity", 0.0f, 100.0f, 1.0f, 50.0f, 0.0f, "%", amp.velSensPct)                     \
  X(ampVolume, "Amp Volume", 0.0f, 100.0f, 1.0f, 80.0f, 0.0f, "%", amp.volPct)                             \
                                                                                                           \
  /* -------------------- LFO 1 -------------------- */                                                    \
  X(lfo1Rate, "LFO 1 Rate", 0.01f, 20.0f, 0.001f, 1.0f, 1.0f, "Hz", lfo1.hz)                               \
  X(lfo1Amplitude, "LFO 1 Amplitude", 0.0f, 100.0f, 1.0f, 100.0f, 0.0f, "%", lfo1.volPct)                  \
  X(lfo1CutoffFreq, "LFO 1 Cutoff Freq", 0.0f, 20000.0f, 0.1f, 0.0f, 200.0f, "Hz", lfo1.cutoffHz)          \
  X(lfo1VoiceFreq, "LFO 1 Voice Freq", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", lfo1.voiceFreqPct)          \
  X(lfo1Shape1, "LFO 1 Shape 1", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", lfo1.shape1Pct)                   \
  X(lfo1AmpVol, "LFO 1 Amp Vol", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", lfo1.ampVolPct)                   \
                                                                                                           \
  /* -------------------- LFO 2 -------------------- */                                                    \
  X(lfo2Rate, "LFO 2 Rate", 0.01f, 20.0f, 0.001f, 1.0f, 1.0f, "Hz", lfo2.hz)                               \
  X(lfo2Amplitude, "LFO 2 Amplitude", 0.0f, 100.0f, 1.0f, 100.0f, 0.0f, "%", lfo2.volPct)                  \
  X(lfo2CutoffFreq, "LFO 2 Cutoff Freq", 0.0f, 20000.0f, 0.1f, 0.0f, 200.0f, "Hz", lfo2.cutoffHz)          \
  X(lfo2VoiceFreq, "LFO 2 Voice Freq", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", lfo2.voiceFreqPct)          \
  X(lfo2Shape1, "LFO 2 Shape 1", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", lfo2.shape1Pct)                   \
  X(lfo2Shape2, "LFO 2 Shape 2", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", lfo2.shape2Pct)                   \
                                                                                                           \
  /* -------------------- Mod Envelope -------------------- */                                             \
  X(modEnvAttack, "Mod Env Attack", 0.001f, 10.0f, 0.001f, 0.1f, 1.0f, "s", modEnv.attackSecs)             \
  X(modEnvDecay, "Mod Env Decay", 0.1f, 10.0f, 0.001f, 0.5f, 1.0f, "s", modEnv.decaySecs)                  \
  X(modEnvSustain, "Mod Env Sustain", 0.0f, 1.0f, 0.001f, 0.5f, 0.5f, "", modEnv.sustainFctr)              \
  X(modEnvRelease, "Mod Env Release", 0.1f, 10.0f, 0.001f, 0.5f, 1.0f, "s", modEnv.releaseSecs)            \
  X(modEnvOsc2Volume, "Mod Env Osc 2 Volume", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", modEnv.osc2VolPct)   \
  X(modEnvVoiceFreq, "Mod Env Voice Freq", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", modEnv.voiceFreqPct)    \
  X(modEnvShape1, "Mod Env Shape 1", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", modEnv.shape1Pct)             \
  X(modEnvShape2, "Mod Env Shape 2", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", modEnv.shape2Pct)             \
                                                                                                           \
  /* -------------------- Velocity -------------------- */                                                 \
  X(velShape1, "Vel Shape 1", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", vel.shape1Pct)                       \
  X(velShape2, "Vel Shape 2", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", vel.shape2Pct)                       \
  X(velCutoff, "Vel Cutoff", -20000.0f, 20000.0f, 1.0f, 0.0f, 0.0f, "Hz", vel.cutoffHz)                    \
  X(velResonance, "Vel Resonance", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", vel.resonancePct)               \
                                                                                                           \
  /* -------------------- Global -------------------- */                                                   \
  X(globalGlide, "Glide", 0.0f, 5.0f, 0.01f, 0.0f, 0.5f, "s", global.glideSecs)                            \
  X(globalTuning, "Global Tuning", 400.0f, 480.0f, 0.1f, 440.0f, 440.0f, "Hz", global.tuningA4Hz)

// Choice parameters: no range, has a list of names.
// Columns: id, display, names (pipe-separated), default index, dest
#define PARAM_CHOICE_LIST                                                                                  \
  X(lfo1Shape, "LFO 1 Shape", "Sine|Tri|Ramp Down|Ramp Up|Square|25% Pulse|10% Pulse", 0, lfo1.waveformId) \
  X(lfo2Shape, "LFO 2 Shape", "Sine|Tri|Ramp Down|Ramp Up|Square|25% Pulse|10% Pulse", 0, lfo2.waveformId)

// Bool parameters: no range, no unit.
// Columns: id, display, default, dest
#define PARAM_BOOL_LIST                                                     \
  X(ampAttackFromZero, "Amp Attack From Zero", false, amp.doAttackFromZero) \
  X(lfo1Keytrig, "LFO 1 Keytrig", false, lfo1.doKeytrig)                    \
  X(lfo2Keytrig, "LFO 2 Keytrig", false, lfo2.doKeytrig)                    \
  X(osc1Keytrig, "OSC 1 Keytrig", true, osc1.doKeytrig)                     \
  X(osc2Keytrig, "OSC 2 Keytrig", true, osc2.doKeytrig)

// Int parameters: min/max integer, step is implied as 1.
// Columns: id, display, min, max, default, dest
#define PARAM_INT_LIST \
  X(globalVoiceCount, "Voice Count", 1, 8, 8, global.voiceCount)