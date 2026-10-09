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
{
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
    params.applyToSynth(synth);  // Takes values and writes to synth
    synth.updateVoicesParams();  // From synth to voices
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

void TccSynthAudioProcessor::handleMIDI(uint8_t data0, uint8_t data1, uint8_t data2) {
  // Print out the MIDI message:
  // char s[16];
  // snprintf(s, 16, "%02hhX %02hhX %02hhX", data0, data1, data2);
  // DBG(s);

  // Control Change
  /*
  if ((data0 & 0xF0) == 0xB0) {
    if (data1 == 0x07) {  // volume
      float volumeCtl = float(data2) / 127.0f;
      outputLevelParam->beginChangeGesture();
      outputLevelParam->setValueNotifyingHost(volumeCtl);
      outputLevelParam->endChangeGesture();
    }
  }

  // Program Change
  if ((data0 & 0xF0) == 0xC0) {
    if (data1 < presets.size()) {
      setCurrentProgram(data1);
    }
  }
  */

  // TODO: Write function that does the dyanmic selection
  // Think of it a virtual cc bank where each CC is hardcoded to a parameter, when a midi note+channel message is sent
  // it tranposese the bank up, ie
  // if (notemessage.channel == 16 AND notemessage.note >= 0 AND
  // notemessage.note < 8) {
  // bank = virtualCC + notemessage.note * 8 }
  // Maybe use a switch statement? That way the user can configure any
  // Midi note instead of having to use 0 to 7

  synth.receiveMidi(data0, data1, data2);
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

    // Render the event (ignore sysex messages which have > 3 bytes)
    if (metadata.numBytes <= 3) {
      uint8_t data1 = (metadata.numBytes >= 2) ? metadata.data[1] : 0;
      uint8_t data2 = (metadata.numBytes == 3) ? metadata.data[2] : 0;
      handleMIDI(metadata.data[0], data1, data2);
    }
  }

  // Render the audio after the last MIDI event. If there were no
  // MIDI events at all, this renders the entire buffer.
  int samplesLastSegment = buffer.getNumSamples() - bufferOffset;
  if (samplesLastSegment > 0) {
    render(buffer, samplesLastSegment, bufferOffset);
  }

  midiMessages.clear();
}

void TccSynthAudioProcessor::render(juce::AudioBuffer<float>& buffer, int sampleCount, int bufferOffset) {
  float* outputBuffers[2] = {nullptr, nullptr};
  outputBuffers[0] = buffer.getWritePointer(0) + bufferOffset;
  if (getTotalNumOutputChannels() > 1) {
    outputBuffers[1] = buffer.getWritePointer(1) + bufferOffset;
  }

  synth.render(outputBuffers, sampleCount);
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