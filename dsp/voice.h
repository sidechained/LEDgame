// ChannelPostProcessor

#ifndef LEDGAME_DSP_VOICE_H_
#define LEDGAME_DSP_VOICE_H_

#include "../stmlib/stmlib.h"

#include "../stmlib/dsp/filter.h"
#include "../stmlib/dsp/limiter.h"

#include "../dsp/envelope.h"

#include "../dsp/fx/low_pass_gate.h"

namespace ledGame {

class ChannelPostProcessor {
 public:
  ChannelPostProcessor() { }
  ~ChannelPostProcessor() { }

  void Init() {
    lpg_.Init();
    Reset();
  }

  void Reset() {
    limiter_.Init();
  }

  void Process(
      float gain,
      bool bypass_lpg,
      float low_pass_gate_gain,
      float low_pass_gate_frequency,
      float low_pass_gate_hf_bleed,
      float* in,
      uint16_t* out,
      size_t size,
      size_t stride) {
    if (gain < 0.0f) {
      limiter_.Process(-gain, in, size);
    }
    const float post_gain = (gain < 0.0f ? 1.0f : gain) * -32767.0f;
    if (!bypass_lpg) {
      lpg_.Process(
          post_gain * low_pass_gate_gain,
          low_pass_gate_frequency,
          low_pass_gate_hf_bleed,
          in,
          out,
          size,
          stride);
    } else {
      while (size--) {
        *out = stmlib::Clip16(1 + static_cast<int32_t>(*in++ * post_gain));
        out += stride;
      }
    }
  }

 private:
  stmlib::Limiter limiter_;
  LowPassGate lpg_;

  DISALLOW_COPY_AND_ASSIGN(ChannelPostProcessor);
};

}  // namespace ledGame

#endif  // LEDGAME_DSP_VOICE_H_
