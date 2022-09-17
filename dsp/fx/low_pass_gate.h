// Approximative low pass gate.

#ifndef LEDGAME_DSP_FX_LOW_PASS_GATE_H_
#define LEDGAME_DSP_FX_LOW_PASS_GATE_H_

#include <algorithm>

#include "../../stmlib/dsp/dsp.h"
#include "../../stmlib/dsp/filter.h"
#include "../../stmlib/dsp/parameter_interpolator.h"

namespace ledGame {

class LowPassGate {
 public:
  LowPassGate() { }
  ~LowPassGate() { }

  void Init() {
    previous_gain_ = 0.0f;
    filter_.Init();
  }

  void Process(
      float gain,
      float frequency,
      float hf_bleed,
      float* in_out,
      size_t size) {
    stmlib::ParameterInterpolator gain_modulation(&previous_gain_, gain, size);
    filter_.set_f_q<stmlib::FREQUENCY_DIRTY>(frequency, 0.4f);
    while (size--) {
      const float s = *in_out * gain_modulation.Next();
      const float lp = filter_.Process<stmlib::FILTER_MODE_LOW_PASS>(s);
      *in_out++ = lp + (s - lp) * hf_bleed;
    }
  }

  void Process(
      float gain,
      float frequency,
      float hf_bleed,
      float* in,
      uint16_t* out,
      size_t size,
      size_t stride) {
    stmlib::ParameterInterpolator gain_modulation(&previous_gain_, gain, size);
    filter_.set_f_q<stmlib::FREQUENCY_DIRTY>(frequency, 0.4f);
    while (size--) {
      const float s = *in++ * gain_modulation.Next();
      const float lp = filter_.Process<stmlib::FILTER_MODE_LOW_PASS>(s);
      *out = stmlib::Clip16(1 + static_cast<int32_t>(lp + (s - lp) * hf_bleed));
      out += stride;
    }
  }

 private:
  float previous_gain_;
  stmlib::Svf filter_;

  DISALLOW_COPY_AND_ASSIGN(LowPassGate);
};

}  // namespace ledGame

#endif  // LEDGAME_DSP_FX_LOW_PASS_GATE_H_
