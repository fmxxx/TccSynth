#pragma once
#include <array>

#include <JuceHeader.h>

#include "ParameterList.h"

class Synth;

struct Entry {
  int cc;  // -1 = no MIDI CC assigned
  const char* paramId;
};

extern const std::array<Entry, 64> CCtable;

// ------ Parameter IDs ------ //
namespace ParameterID {
#define PARAMETER_ID(str) const juce::ParameterID str(#str, 1);
#define X(id, ...) PARAMETER_ID(id)
PARAM_FLOAT_LIST
PARAM_CHOICE_LIST
PARAM_BOOL_LIST
PARAM_INT_LIST
#undef X
#undef PARAMETER_ID
}  // namespace ParameterID

// ------ ParameterHandler ------ //
class ParameterHandler : private juce::ValueTree::Listener {
 public:
  explicit ParameterHandler(juce::AudioProcessor& processor);
  ~ParameterHandler() override;

  // Set to true when any parameter changes. Read by the processor.
  std::atomic<bool> parametersChanged{false};

  // Read every parameter and push it into the synth's public fields.
  // Called from the audio thread (inside processBlock).
  void applyToSynth(Synth& synth);

  // APVTS is public so the processor can save/load state and the editor
  // can create attachments.
  juce::AudioProcessorValueTreeState apvts;

  // Float pointer members
#define X(id, ...) juce::AudioParameterFloat* id##Param = nullptr;
  PARAM_FLOAT_LIST
#undef X

// Choice pointer members
#define X(id, ...) juce::AudioParameterChoice* id##Param = nullptr;
  PARAM_CHOICE_LIST
#undef X

// Bool pointer members
#define X(id, ...) juce::AudioParameterBool* id##Param = nullptr;
  PARAM_BOOL_LIST
#undef X

// Int pointer members
#define X(id, ...) juce::AudioParameterInt* id##Param = nullptr;
  PARAM_INT_LIST
#undef X

 private:
  juce::AudioProcessor& processor;

  juce::AudioProcessorValueTreeState::ParameterLayout createLayout();
  void castAllParameters();

  void valueTreePropertyChanged(juce::ValueTree&,
                                const juce::Identifier&) override {
    parametersChanged.store(true);
  }
};