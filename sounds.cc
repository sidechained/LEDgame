#include "sounds.h"

using namespace std;

namespace ledGame {

// base Sound class functions:

void Sound::Init() {
  decayEnvelope.Init();
  lPostProcessor.Init();
  rPostProcessor.Init();
}

void Sound::Trigger() {
  decayEnvelope.Trigger();
}

void Sound::PostProcess(AudioDac::Frame* output, size_t size) {
  decayEnvelope.Process(dcay);
  lPostProcessor.Process( // lpg plus gain, limiting, clipping and float to short conv
    1.0f, // gain
    false, // bypass_lpg
    decayEnvelope.value(), //low_pass_gate_gain
    0.8f, //low_pass_gate_frequency,
    0.5f, //low_pass_gate_hf_bleed,
    input,
    &output->l,
    size,
    2 // stride
  );
  rPostProcessor.Process( // lpg plus gain, limiting, clipping and float to short conv
    1.0f, // gain
    false, // bypass_lpg
    decayEnvelope.value(), //low_pass_gate_gain
    0.8f, //low_pass_gate_frequency,
    0.5f, //low_pass_gate_hf_bleed,
    input,
    &output->r,
    size,
    2 // stride
  );
}

// LoseSound functions:

void WinSound::Init() {
  oscillator.Init();
  dcay = (float(kBlockSize) / kSampleRate) * 7.0f;
  Sound::Init();
}

void WinSound::Trigger() {
  f = 224.0f / 48000.0f;
  Sound::Trigger();
}

void WinSound::RenderWinRound(AudioDac::Frame* output, size_t size) {
  oscillator.Render<OSCILLATOR_SHAPE_SLOPE>(f, 0.5f, input, size);
  PostProcess(output, size);
}

void WinSound::RenderWinGame(AudioDac::Frame* output, size_t size) {
  dcay = (float(kBlockSize) / kSampleRate) * 7.0f;
  oscillator.Render<OSCILLATOR_SHAPE_SLOPE>(f=f*1.001f, 0.5f, input, size);
  PostProcess(output, size);
}

// WinSound functions:

void LoseSound::Init() {
  harmonicOscillator.Init();
  f0 = 2.0f / kSampleRate;
  fill(&amplitudes[0], &amplitudes[16], 0.0f);
  amplitudes[15] = 0.10f;
  dcay = (float(kBlockSize) / kSampleRate) * 7.0f;
  Sound::Init();
}

void LoseSound::Render(AudioDac::Frame* output, size_t size) {
  harmonicOscillator.Render<8>(f0, amplitudes, input, size);
  PostProcess(output, size);
}

}  // namespace ledGame
