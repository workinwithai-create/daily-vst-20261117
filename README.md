# Today’s Instrument: November Resonance

A polyphonic cinematic pad synthesizer with aftertouch-driven filter expression and sustain pedal support for live, breathing performances.

## Overview

November Resonance is a VST3 / Standalone instrument built with JUCE 8 and C++20. It features:

- 8-voice polyphony using `juce::Synthesiser`
- Saw/sine hybrid oscillator (sine for purity with filter for character)
- State-variable filter with aftertouch modulation of cutoff
- ADSR envelope
- Sustain pedal (CC64) support via Synthesiser
- Channel aftertouch response
- Safety limiter to prevent clipping
- Parameter smoothing
- Full state save/load via APVTS
- Dark cinematic UI

## Building

Requirements: CMake 3.22+, C++20 compiler, Git.

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

Artefacts appear in `build/NovemberResonance_artefacts/Release/`.

- VST3: `VST3/November Resonance.vst3`
- Standalone: `Standalone/November Resonance`

## MIDI

- Note on/off for polyphony
- Channel aftertouch (pressure) modulates filter cutoff (amount controlled by parameter)
- Sustain pedal (CC 64) holds notes

## UI

Dark cinematic theme: deep navy/black background, subtle purple accents, clean labels for Cutoff, Resonance, Attack, Decay, Sustain, Release, Aftertouch Amount.

## License

MIT (for this project). JUCE is under its own license.
