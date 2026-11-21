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
//   numID       - index in the 64-slot MIDI CC table (-1 = no relative-control slot)
//   CC          - MIDI CC number (-1 = no MIDI CC assigned)
//
// To add a parameter: append one X(...) line here, then add one field to the
// matching struct in SynthParams. Everything else expands automatically.
// ------------------------====

// id, display, min, max, step, default, centre, unit, dest, numID, CC

#define PARAM_FLOAT_LIST                                                                                   \
  /* ------Oscillators ------- */                                                                          \
  X(osc1Semitone, "Osc 1 Semitone", -48.0f, 48.0f, 1.0f, 0.0f, 0.0f, "semi", osc1.semitones, 0, 14)       \
  X(osc1Cents, "Osc 1 Cents", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "cent", osc1.cents, 1, 15)               \
  X(osc1Shape, "Osc 1 Shape", -1.0f, 1.0f, 0.0001f, 0.0f, 0.0f, "", osc1.shapeFctr, 2, 12)                 \
  X(osc1Volume, "Osc 1 Volume", 0.0f, 100.0f, 1.0f, 100.0f, 0.0f, "%", osc1.volPct, 3, 70)                 \
  X(osc2Semitone, "Osc 2 Semitone", -48.0f, 48.0f, 1.0f, 0.0f, 0.0f, "semi", osc2.semitones, 4, 16)       \
  X(osc2Cents, "Osc 2 Cents", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "cent", osc2.cents, 5, 17)               \
  X(osc2Shape, "Osc 2 Shape", -1.0f, 1.0f, 0.0001f, 0.0f, 0.0f, "", osc2.shapeFctr, 6, 13)                 \
  X(osc2Volume, "Osc 2 Volume", 0.0f, 100.0f, 1.0f, 100.0f, 0.0f, "%", osc2.volPct, 7, 85)                 \
                                                                                                           \
  /* -------------------- Filter -------------------- */                                                   \
  X(filterAttack, "Filter Attack", 0.001f, 10.0f, 0.001f, 0.1f, 1.0f, "s", filter.attackSecs, 12, 73)      \
  X(filterDecay, "Filter Decay", 0.1f, 10.0f, 0.001f, 1.0f, 1.0f, "s", filter.decaySecs, 13, 75)          \
  X(filterSustain, "Filter Sustain", 0.0f, 1.0f, 0.001f, 0.5f, 0.5f, "", filter.sustainFctr, 14, 79)       \
  X(filterRelease, "Filter Release", 0.1f, 10.0f, 0.001f, 1.0f, 1.0f, "s", filter.releaseSecs, 15, 72)     \
  X(filterCutoff, "Filter Cutoff", 20.0f, 20000.0f, 0.1f, 20000.0f, 1000.0f, "Hz", filter.cutoffHz, 8, 74) \
  X(filterResonance, "Filter Resonance", 0.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", filter.resonancePct, 9, 71)  \
  X(filterKeytrack, "Filter Keytrack", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", filter.keytrackPct, 10, 18) \
  X(filterEnvAmount, "Filter Env Amount", -20000.0f, 20000.0f, 1.0f, 0.0f, 0.0f, "Hz", filter.envAmountHz, 11, 19) \
                                                                                                           \
  /* -------------------- Amp -------------------- */                                                      \
  X(ampAttack, "Amp Attack", 0.001f, 10.0f, 0.001f, 0.1f, 1.0f, "s", amp.attackSecs, 20, 44)               \
  X(ampDecay, "Amp Decay", 0.1f, 10.0f, 0.001f, 0.5f, 1.0f, "s", amp.decaySecs, 21, 45)                    \
  X(ampSustain, "Amp Sustain", 0.0f, 1.0f, 0.001f, 0.5f, 0.5f, "", amp.sustainFctr, 22, 46)                \
  X(ampRelease, "Amp Release", 0.1f, 10.0f, 0.001f, 0.5f, 1.0f, "s", amp.releaseSecs, 23, 47)              \
  X(ampVelocity, "Amp Velocity", 0.0f, 100.0f, 1.0f, 50.0f, 0.0f, "%", amp.velSensPct, 17, 11)             \
  X(ampVolume, "Amp Volume", 0.0f, 100.0f, 1.0f, 80.0f, 0.0f, "%", amp.volPct, 16, 7)                       \
                                                                                                           \
  /* -------------------- LFO 1 -------------------- */                                                    \
  X(lfo1Rate, "LFO 1 Rate", 0.01f, 20.0f, 0.001f, 1.0f, 1.0f, "Hz", lfo1.hz, 34, 76)                      \
  X(lfo1Amplitude, "LFO 1 Amplitude", 0.0f, 100.0f, 1.0f, 100.0f, 0.0f, "%", lfo1.volPct, 32, 1)          \
  X(lfo1CutoffFreq, "LFO 1 Cutoff Freq", 0.0f, 20000.0f, 0.1f, 0.0f, 200.0f, "Hz", lfo1.cutoffHz, 33, 21) \
  X(lfo1VoiceFreq, "LFO 1 Voice Freq", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", lfo1.voiceFreqPct, 38, 22) \
  X(lfo1Shape1, "LFO 1 Shape 1", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", lfo1.shape1Pct, 36, 23)        \
  X(lfo1AmpVol, "LFO 1 Amp Vol", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", lfo1.ampVolPct, 37, 24)         \
                                                                                                           \
  /* -------------------- LFO 2 -------------------- */                                                    \
  X(lfo2Rate, "LFO 2 Rate", 0.01f, 20.0f, 0.001f, 1.0f, 1.0f, "Hz", lfo2.hz, 42, 77)                      \
  X(lfo2Amplitude, "LFO 2 Amplitude", 0.0f, 100.0f, 1.0f, 100.0f, 0.0f, "%", lfo2.volPct, 40, 2)          \
  X(lfo2CutoffFreq, "LFO 2 Cutoff Freq", 0.0f, 20000.0f, 0.1f, 0.0f, 200.0f, "Hz", lfo2.cutoffHz, 41, 25) \
  X(lfo2VoiceFreq, "LFO 2 Voice Freq", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", lfo2.voiceFreqPct, 46, 26) \
  X(lfo2Shape1, "LFO 2 Shape 1", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", lfo2.shape1Pct, 44, 27)        \
  X(lfo2Shape2, "LFO 2 Shape 2", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", lfo2.shape2Pct, 45, 28)        \
                                                                                                           \
  /* -------------------- Mod Envelope -------------------- */                                             \
  X(modEnvAttack, "Mod Env Attack", 0.001f, 10.0f, 0.001f, 0.1f, 1.0f, "s", modEnv.attackSecs, 28, 29)     \
  X(modEnvDecay, "Mod Env Decay", 0.1f, 10.0f, 0.001f, 0.5f, 1.0f, "s", modEnv.decaySecs, 29, 30)          \
  X(modEnvSustain, "Mod Env Sustain", 0.0f, 1.0f, 0.001f, 0.5f, 0.5f, "", modEnv.sustainFctr, 30, 31)      \
  X(modEnvRelease, "Mod Env Release", 0.1f, 10.0f, 0.001f, 0.5f, 1.0f, "s", modEnv.releaseSecs, 31, 33)    \
  X(modEnvOsc2Volume, "Mod Env Osc 2 Volume", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", modEnv.osc2VolPct, 24, 34) \
  X(modEnvVoiceFreq, "Mod Env Voice Freq", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", modEnv.voiceFreqPct, 25, 35) \
  X(modEnvShape1, "Mod Env Shape 1", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", modEnv.shape1Pct, 26, 36)   \
  X(modEnvShape2, "Mod Env Shape 2", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", modEnv.shape2Pct, 27, 37)   \
                                                                                                           \
  /* -------------------- Velocity -------------------- */                                                 \
  X(velShape1, "Vel Shape 1", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", vel.shape1Pct, 50, 38)              \
  X(velShape2, "Vel Shape 2", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", vel.shape2Pct, 51, 39)              \
  X(velCutoff, "Vel Cutoff", -20000.0f, 20000.0f, 1.0f, 0.0f, 0.0f, "Hz", vel.cutoffHz, 48, 40)           \
  X(velResonance, "Vel Resonance", -100.0f, 100.0f, 1.0f, 0.0f, 0.0f, "%", vel.resonancePct, 49, 41)      \
                                                                                                           \
  /* -------------------- Global -------------------- */                                                   \
  X(globalGlide, "Glide", 0.0f, 5.0f, 0.01f, 0.0f, 0.5f, "s", global.glideSecs, 56, 5)                    \
  X(globalTuning, "Global Tuning", 400.0f, 480.0f, 0.1f, 440.0f, 440.0f, "Hz", global.tuningA4Hz, 57, 20)

// Choice parameters: no range, has a list of names.
// Columns: id, display, names (pipe-separated), default index, dest, numID, CC
#define PARAM_CHOICE_LIST                                                                                       \
  X(lfo1Shape, "LFO 1 Shape", "Sine|Tri|Ramp Down|Ramp Up|Square|25% Pulse|10% Pulse", 0, lfo1.waveformId, 35, 42) \
  X(lfo2Shape, "LFO 2 Shape", "Sine|Tri|Ramp Down|Ramp Up|Square|25% Pulse|10% Pulse", 0, lfo2.waveformId, 43, 43)

// Bool parameters: no range, no unit.
// Columns: id, display, default, dest, numID, CC
#define PARAM_BOOL_LIST                                                                \
  X(ampAttackFromZero, "Amp Attack From Zero", false, amp.doAttackFromZero, 19, 80)    \
  X(lfo1Keytrig, "LFO 1 Keytrig", false, lfo1.doKeytrig, 39, 81)                      \
  X(lfo2Keytrig, "LFO 2 Keytrig", false, lfo2.doKeytrig, 47, 82)                      \
  X(osc1Keytrig, "OSC 1 Keytrig", true, osc1.doKeytrig, 60, 83)                       \
  X(osc2Keytrig, "OSC 2 Keytrig", true, osc2.doKeytrig, 61, 86)

// Int parameters: min/max integer, step is implied as 1.
// Columns: id, display, min, max, default, dest, numID, CC
#define PARAM_INT_LIST \
  X(globalVoiceCount, "Voice Count", 1, 8, 8, global.voiceCount, 58, 87)

// Extra CC-table slots that alias a parameter already declared above.
// Columns: id, numID, CC
#define PARAM_CC_ALIAS_LIST \
  X(ampAttackFromZero, 59, 80)