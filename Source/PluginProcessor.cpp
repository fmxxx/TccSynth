#include "PluginProcessor.h"

#include <JucePluginDefines.h>

#include "PluginEditor.h"

// Constructor
TccSynthAudioProcessor::TccSynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
                         )
#endif
      ,
      midiHandler(synth, params.apvts) {
  // All parameter construction, registration and casting is done by
  // ParameterHandler's own constructor. Nothing to do here.
}

// Destructor
TccSynthAudioProcessor::~TccSynthAudioProcessor() = default;

// ------ Plugin Settings ------ //
const juce::String TccSynthAudioProcessor::getName() const { return JucePlugin_Name; }

bool TccSynthAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool TccSynthAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool TccSynthAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double TccSynthAudioProcessor::getTailLengthSeconds() const { return 0.0; }

// ------ Programs (presets) ------ //
int TccSynthAudioProcessor::getNumPrograms() { return 1; }
int TccSynthAudioProcessor::getCurrentProgram() { return 0; }
void TccSynthAudioProcessor::setCurrentProgram(int) {}
const juce::String TccSynthAudioProcessor::getProgramName(int) { return {}; }
void TccSynthAudioProcessor::changeProgramName(int, const juce::String&) {}

// ------ ------ ------ //
void TccSynthAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
  synth.allocateResources(sampleRate, samplesPerBlock);

  // Force a full parameter push on the first processBlock, so the synth
  // starts with the correct values rather than its hardcoded defaults.
  params.parametersChanged.store(true);
}

void TccSynthAudioProcessor::releaseResources() {
  synth.deallocateResources();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TccSynthAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
#if JucePlugin_IsMidiEffect
  juce::ignoreUnused(layouts);
  return true;
#else
  if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
      layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    return false;
#if !JucePlugin_IsSynth
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    return false;
#endif
  return true;
#endif
}
#endif

// ------ Audio Processing ------ //
void TccSynthAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                          juce::MidiBuffer& midiMessages) {
  juce::ScopedNoDenormals noDenormals;

  // Clear any output channels that don't have input data.
  for (auto i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
    buffer.clear(i, 0, buffer.getNumSamples());

  // Apply parameter changes once per block, before rendering.
  bool expected = true;
  if (params.parametersChanged.compare_exchange_strong(expected, false)) {
    params.applyToSynth(synth);
    synth.updateVoices();
  }

  // Get write pointers for the synth's output. The synth works in-place
  // on the buffer.
  float* outputBuffers[2] = {nullptr, nullptr};
  outputBuffers[0] = buffer.getWritePointer(0);
  if (getTotalNumOutputChannels() > 1)
    outputBuffers[1] = buffer.getWritePointer(1);

  // Render audio and dispatch MIDI.
  splitBufferByEvents(buffer, midiMessages);
}

void TccSynthAudioProcessor::splitBufferByEvents(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
  int bufferOffset = 0;

  // Loop through the MIDI messages, which are sorted by samplePosition,
  // the relative timestamp inside the current audio buffer.
  for (const auto metadata : midiMessages) {
    // Render the audio that happens before this event (if any).
    int samplesThisSegment = metadata.samplePosition - bufferOffset;
    if (samplesThisSegment > 0) {
      render(buffer, samplesThisSegment, bufferOffset);
      bufferOffset += samplesThisSegment;
    }

    midiHandler.handleMidiMetadata(metadata);
  }

  // Render the audio after the last MIDI event. If there were no
  // MIDI events at all, this renders the entire buffer.
  int samplesLastSegment = buffer.getNumSamples() - bufferOffset;
  if (samplesLastSegment > 0) {
    render(buffer, samplesLastSegment, bufferOffset);
  }

  midiMessages.clear();
}

// ------ Creates UI editor if it has one ------ //
bool TccSynthAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* TccSynthAudioProcessor::createEditor() {
  return new TccSynthAudioProcessorEditor(*this);
}

// ------ Save and load plugin state ------ //
void TccSynthAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
  copyXmlToBinary(*params.apvts.copyState().createXml(), destData);
}

void TccSynthAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
  std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
  if (xml.get() != nullptr && xml->hasTagName(params.apvts.state.getType())) {
    params.apvts.replaceState(juce::ValueTree::fromXml(*xml));
    params.parametersChanged.store(true);
  }
}

// ------ ------ ------ //
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
  return new TccSynthAudioProcessor();
}