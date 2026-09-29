#pragma once

#include <JuceHeader.h>

#include <array>

#include "Synth.h"

class MidiHandler {
 public:
  MidiHandler(Synth& synth, juce::AudioProcessorValueTreeState& apvts);

  void handleMidiMetadata(const juce::MidiMessageMetadata& metadata);

  void setKeyboardChannel(int ch);
  void setControllerChannel(int ch);

 private:
  void handleKeyboard(int note, int vel);
  void handlePad(int note);
  void handleEncoder(int cc, int value);

  static constexpr int NUM_MODULES = 8;

  Synth& synth;
  juce::AudioProcessorValueTreeState& apvts;

  int keyboardChannel = 1;
  int controllerChannel = 2;
  int currentModule = 0;

  std::array<std::array<juce::RangedAudioParameter*, 8>, NUM_MODULES> encoderMap;
};