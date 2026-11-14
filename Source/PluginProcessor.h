#pragma once
#include <JuceHeader.h>

#include "Framework/ParameterHandler.h"
#include "Framework/Synth.h"

class TccSynthAudioProcessor : public juce::AudioProcessor,
                               private juce::Timer {
 public:
  TccSynthAudioProcessor();
  ~TccSynthAudioProcessor() = default;

  /// ------ ------ ------ //
  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
  bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

  void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

  // ------ ------ ------ //
  juce::AudioProcessorEditor* createEditor() override;
  bool hasEditor() const override;

  // ------ ------ ------ //
  const juce::String getName() const override;
  bool acceptsMidi() const override;
  bool producesMidi() const override;
  bool isMidiEffect() const override;
  double getTailLengthSeconds() const override;

  // ------ ------ ------ //
  int getNumPrograms() override;
  int getCurrentProgram() override;
  void setCurrentProgram(int index) override;
  const juce::String getProgramName(int index) override;
  void changeProgramName(int index, const juce::String& newName) override;

  // ------ ------ ------ //
  void getStateInformation(juce::MemoryBlock& destData) override;
  void setStateInformation(const void* data, int sizeInBytes) override;

  void timerCallback() override;

  // Accessors for the editor
  ParameterHandler& getParameters() { return params; }
  Synth& getSynth() { return synth; }

 private:
  // Declare in order: ParameterHandler -> Synth
  ParameterHandler params{*this};
  Synth synth;

  // ------ MIDI CC ------ //
  // ccParamMap[cc]   = parameter that CC controls (nullptr = unmapped)
  // ccIsBool[cc]     = true if the parameter is a bool (>=64 threshold)
  // pendingCC[cc]    = pending normalized value (>=0), -1.0f = "no pending update"
  std::array<juce::AudioProcessorParameter*, 128> ccParamMap{};
  std::array<bool, 128> ccIsBool{};
  std::array<std::atomic<float>, 128> pendingCC;

  void setupCCMapping();

  void splitBufferByEvents(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);
  void handleMIDI(uint8_t data0, uint8_t data1, uint8_t data2);
  void render(juce::AudioBuffer<float>& buffer, int sampleCount, int bufferOffset);

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TccSynthAudioProcessor)
};