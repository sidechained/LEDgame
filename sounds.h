#ifndef LEDGAME_SOUNDS_H_
#define LEDGAME_SOUNDS_H_

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
//#include <xmmintrin.h>

#include "drivers/audio_dac.h"
#include "dsp/oscillator/oscillator.h"
#include "dsp/oscillator/harmonic_oscillator.h"
#include "dsp/envelope.h"
#include "dsp/voice.h"
#include "dsp/dsp.h"

namespace ledGame {

class Sound {
  public:
    Sound() { }
    ~Sound() { }

    AudioDac audio_dac;
    DecayEnvelope decayEnvelope;
    ChannelPostProcessor lPostProcessor;
    ChannelPostProcessor rPostProcessor;

    float input[kBlockSize];
    float dcay;

    void Init();
    void Trigger();
    void PostProcess(AudioDac::Frame* output, size_t size);

  private:
    // nothing here

  DISALLOW_COPY_AND_ASSIGN(Sound);
};

class WinSound : public Sound {
  public:
    WinSound() { }
    ~WinSound() { }

    AudioDac audio_dac;

    void Init();
    void Trigger();
    void RenderWinRound(AudioDac::Frame* output, size_t size);
    void RenderWinGame(AudioDac::Frame* output, size_t size);

  private:
    Oscillator oscillator;
    float f;

  DISALLOW_COPY_AND_ASSIGN(WinSound);
};

class LoseSound : public Sound {
  public:
    LoseSound() { }
    ~LoseSound() { }

    AudioDac audio_dac;

    void Init();
    void Render(AudioDac::Frame* output, size_t size);

  private:
    HarmonicOscillator<16> harmonicOscillator;

    float f0;
    float amplitudes[16];

  DISALLOW_COPY_AND_ASSIGN(LoseSound);
};

}  // namespace ledGame

#endif  // LEDGAME_SOUNDS_H_
