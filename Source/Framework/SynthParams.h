// This class contains the variables for each parameter
// To be used in Synth.cpp and passed down to the voices

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
    bool keytrig;
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
