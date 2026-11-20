#pragma once

struct Entry {
  int cc;  // -1 = no MIDI CC assigned (placeholder or unmapped)
  const char* paramId;
};
static const Entry CCtable[] = {
    // Osc 1 and 2
    {14, "osc1Semitone"},
    {15, "osc1Cents"},
    {12, "osc1Shape"},
    {70, "osc1Volume"},
    {16, "osc2Semitone"},
    {17, "osc2Cents"},
    {13, "osc2Shape"},
    {85, "osc2Volume"},

    // Filter
    {74, "filterCutoff"},
    {71, "filterResonance"},
    {18, "filterKeytrack"},
    {19, "filterEnvAmount"},
    {73, "filterAttack"},
    {75, "filterDecay"},
    {79, "filterSustain"},
    {72, "filterRelease"},

    // Amp
    {7, "ampVolume"},
    {11, "ampVelocity"},
    {-1, "noiseVol"},  // placeholder
    {80, "ampAttackFromZero"},
    {44, "ampAttack"},
    {45, "ampDecay"},
    {46, "ampSustain"},
    {47, "ampRelease"},

    // Mod Env
    {34, "modEnvOsc2Volume"},
    {35, "modEnvVoiceFreq"},
    {36, "modEnvShape1"},
    {37, "modEnvShape2"},
    {29, "modEnvAttack"},
    {30, "modEnvDecay"},
    {31, "modEnvSustain"},
    {33, "modEnvRelease"},

    // Lfo 1
    {1, "lfo1Amplitude"},
    {21, "lfo1CutoffFreq"},
    {76, "lfo1Rate"},
    {42, "lfo1Shape"},
    {23, "lfo1Shape1"},
    {24, "lfo1Shape2"},
    {22, "lfo1VoiceFreq"},
    {81, "lfo1Keytrig"},

    // Lfo 2
    {2, "lfo2Amplitude"},
    {25, "lfo2CutoffFreq"},
    {77, "lfo2Rate"},
    {43, "lfo2Shape"},
    {27, "lfo2Shape1"},
    {28, "lfo2Shape2"},
    {26, "lfo2VoiceFreq"},
    {82, "lfo2Keytrig"},

    // Velocity
    {40, "velCutoff"},
    {41, "velResonance"},
    {38, "velShape1"},
    {39, "velShape2"},
    {-1, ""},  // placeholder
    {-1, ""},  // placeholder
    {-1, ""},  // placeholder
    {-1, ""},  // placeholder

    // Global
    {5, "globalGlide"},
    {20, "globalTuning"},
    {87, "globalVoiceCount"},
    {80, "ampAttackFromZero"},
    {83, "osc1Keytrig"},
    {86, "osc2Keytrig"},
    {-1, ""},  // placeholder
    {-1, ""},  // placeholder
};