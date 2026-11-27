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
  for (const auto& e : CCtable) {
    if (e.cc < 0 || e.cc > 127) continue;  // skip placeholders
    if (e.paramId == nullptr || e.paramId[0] == '\0') continue;
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
  constexpr int BANK_CHANNEL = 10;
  constexpr int BANK_MIN_NOTE = 36;  // C1
  constexpr int BANK_MAX_NOTE = 43;
  constexpr int RELATIVE_CC_START = 100;
  constexpr int RELATIVE_CC_END = 107;
  constexpr int CCS_PER_BANK = RELATIVE_CC_END - RELATIVE_CC_START + 1;

  // Print out the MIDI message:
  // char s[64];
  // uint8_t status = data0 & 0xF0;         // Upper nibble (command)
  // uint8_t channel = (data0 & 0x0F) + 1;  // Lower nibble (1-16)

  // if (status == 0xB0) {
  //   snprintf(s, sizeof(s), "CC | Ch:%d Num:%d Val:%d (%02hhX %02hhX %02hhX)",
  //            channel, data1, data2, data0, data1, data2);
  // } else {
  //   snprintf(s, sizeof(s), "Ch:%d (%02hhX %02hhX %02hhX)",
  //            channel, data0, data1, data2);
  // }

  juce::MidiMessage msg(data0, data1, data2);

  if (msg.getChannel() == BANK_CHANNEL) {
    int note = msg.getNoteNumber();
    DBG(note);
    if (note >= BANK_MIN_NOTE && note <= BANK_MAX_NOTE) {
      bank = msg.getNoteNumber() - BANK_MIN_NOTE;
    }
  }

  // CC mapped to a parameter -> store pending value, consume the message.
  // Unmapped CCs fall through to synth.receiveMidi (no-op today, useful later).
  if ((data0 & 0xF0) == 0xB0) {
    const int cc = data1 & 0x7F;
    const int value = data2 & 0x7F;

    if (cc >= RELATIVE_CC_START && cc <= RELATIVE_CC_END) {
      // Binary offset
      int CCid = (cc - RELATIVE_CC_START) + bank * CCS_PER_BANK;

      if (value > 64) {
        nudgeParameter(CCtable[CCid].cc, +1);
      } else if (value < 64) {
        nudgeParameter(CCtable[CCid].cc, -1);
      }
    } else {
      // All other CCs: absolute
      if (ccIsBool[cc]) {
        pendingCC[cc].store(value >= 64 ? 1.0f : 0.0f);
      } else {
        pendingCC[cc].store(value / 128.0f);
      }
    }
    return;
  }

  if (msg.getChannel() == BANK_CHANNEL) return;

  synth.receiveMidi(data0, data1, data2);
}

void TccSynthAudioProcessor::nudgeParameter(int cc, int steps) {
  if (cc < 0 || cc > 127) return;
  if (ccParamMap[cc] == nullptr) return;
  if (steps == 0) return;

  constexpr float STEP = 1.0f / 127.0f;

  // Base = pending if a click already arrived this block, else current param.
  float base = pendingCC[cc].load();
  if (base < 0.0f) base = ccParamMap[cc]->getValue();

  const float newVal = std::clamp(base + static_cast<float>(steps) * STEP,
                                  0.0f, 1.0f);

  if (ccIsBool[cc]) {
    pendingCC[cc].store(newVal >= 0.5f ? 1.0f : 0.0f);
  } else {
    pendingCC[cc].store(newVal);
  }
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