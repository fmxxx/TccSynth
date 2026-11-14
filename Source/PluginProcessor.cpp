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
  // ParameterHandler's own constructor.

  // Initialize CC pending array: -1 = "no pending update"
  for (auto& a : pendingCC) a.store(-1.0f);

  setupCCMapping();
}

void TccSynthAudioProcessor::setupCCMapping() {
  struct Entry {
    int cc;
    const char* paramId;
  };
  static const Entry table[] = {
      {1, "lfo1Amplitude"},
      {2, "lfo2Amplitude"},
      {5, "globalGlide"},
      {7, "ampVolume"},
      {11, "ampVelocity"},
      {12, "osc1Shape"},
      {13, "osc2Shape"},
      {14, "osc1Semitone"},
      {15, "osc1Cents"},
      {16, "osc2Semitone"},
      {17, "osc2Cents"},
      {18, "filterKeytrack"},
      {19, "filterEnvAmount"},
      {20, "globalTuning"},
      {21, "lfo1CutoffFreq"},
      {22, "lfo1VoiceFreq"},
      {23, "lfo1Shape1"},
      {24, "lfo1Shape2"},
      {25, "lfo2CutoffFreq"},
      {26, "lfo2VoiceFreq"},
      {27, "lfo2Shape1"},
      {28, "lfo2Shape2"},
      {29, "modEnvAttack"},
      {30, "modEnvDecay"},
      {31, "modEnvSustain"},
      {33, "modEnvRelease"},
      {34, "modEnvOsc2Volume"},
      {35, "modEnvVoiceFreq"},
      {36, "modEnvShape1"},
      {37, "modEnvShape2"},
      {38, "velShape1"},
      {39, "velShape2"},
      {40, "velCutoff"},
      {41, "velResonance"},
      {42, "lfo1Shape"},
      {43, "lfo2Shape"},
      {44, "ampAttack"},
      {45, "ampDecay"},
      {46, "ampSustain"},
      {47, "ampRelease"},
      {70, "osc1Volume"},
      {71, "filterResonance"},
      {72, "filterRelease"},
      {73, "filterAttack"},
      {74, "filterCutoff"},
      {75, "filterDecay"},
      {76, "lfo1Rate"},
      {77, "lfo2Rate"},
      {79, "filterSustain"},
      {80, "ampAttackFromZero"},
      {81, "lfo1Keytrig"},
      {82, "lfo2Keytrig"},
      {83, "osc1Keytrig"},
      {85, "osc2Volume"},
      {86, "osc2Keytrig"},
      {87, "globalVoiceCount"},
  };

  for (const auto& e : table) {
    if (auto* p = params.apvts.getParameter(e.paramId)) {
      ccParamMap[e.cc] = p;
      ccIsBool[e.cc] = (dynamic_cast<juce::AudioParameterBool*>(p) != nullptr);
    } else {
      DBG("setupCCMapping: parameter not found: " << e.paramId);
    }
  }
}

// Destructor
// TccSynthAudioProcessor::~TccSynthAudioProcessor() = default;

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

  startTimerHz(60);
}

void TccSynthAudioProcessor::releaseResources() {
  stopTimer();
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

  // CC mapped to a parameter → store pending value, consume the message.
  // Unmapped CCs fall through to synth.receiveMidi (no-op today, useful later).
  if ((data0 & 0xF0) == 0xB0) {
    const int cc = data1 & 0x7F;
    if (ccParamMap[cc] != nullptr) {
      const int value = data2 & 0x7F;
      if (ccIsBool[cc]) {
        // Standard MIDI switch: 0-63 off, 64-127 on.
        pendingCC[cc].store(value >= 64 ? 1.0f : 0.0f);
      } else {
        // /128: 128 values spread evenly, CC 64 = exactly 0.5 (the middle).
        pendingCC[cc].store(value / 128.0f);
      }
      return;
    }
  }
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

void TccSynthAudioProcessor::timerCallback() {
  for (int cc = 0; cc < 128; ++cc) {
    if (ccParamMap[cc] == nullptr) continue;

    const float v = pendingCC[cc].load();
    if (v < 0.0f) continue;

    pendingCC[cc].store(-1.0f);                // consume
    ccParamMap[cc]->setValueNotifyingHost(v);  // notify host + UI + param pipeline
  }
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