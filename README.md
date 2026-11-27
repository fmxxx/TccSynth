A virtual analog Synthesizer that can be controlled with only 8 endless encoders and 8 buttons. Each button selects a "page" such as oscillator, filter, lfo, etc.
Work in progress for my undergraduate thesis.

Built with C++ and the JUCE framework.

## Usage

MIDI CC# 100 - 107 when set to relative mode will adapt to whichever page is selected. For that to work the need to be configured so that clockwise turning emits any CC Value equal or grater than 65 and
Counter-clockwise emits values equal or less than 63.

MIDI notes 36 - 43 sent on channel 10 let you switch between 8 pages (Oscillators, Filter, Amp, Mod Envelope, LFO 1, LFO2, Velocity modulation, Global).

MIDI CC# 0 to 99 work just like any other MIDI CC.
