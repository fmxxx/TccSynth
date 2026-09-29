#include "MidiHandler.h"

MidiHandler::MidiHandler(Synth& s, juce::AudioProcessorValueTreeState& a)
    : synth(s), apvts(a) {
}

void MidiHandler::handleMidiMetadata(const juce::MidiMessageMetadata& metadata) {
  if (metadata.numBytes > 3)
    return;  // ignore sysex

  uint8_t data0 = metadata.data[0];
  uint8_t data1 = (metadata.numBytes >= 2) ? metadata.data[1] : 0;
  uint8_t data2 = (metadata.numBytes == 3) ? metadata.data[2] : 0;

  // TODO: volume / expression / program change CCs

  synth.receiveMidi(data0, data1, data2);
}

void MidiHandler::setKeyboardChannel(int ch) { keyboardChannel = ch; }
void MidiHandler::setControllerChannel(int ch) { controllerChannel = ch; }