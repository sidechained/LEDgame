#include "ledGame.h"
#include "drivers/audio_dac.h"
#include "noise.h"

using namespace std;
using namespace ledGame;

FAUSTCLASS mygreatdsp;
AudioDac audio_dac;

static float output0[kBlockSize] = {0.0};
static float output1[kBlockSize] = {0.0};
static float* output[2] = {output0, output1};

extern "C" {
  // Cortex-M4 Processor Exceptions Handlers:
  void NMI_Handler(void) { }
  void HardFault_Handler(void) { while (1) { } }
  void MemManage_Handler(void) { while (1) { } }
  void BusFault_Handler(void) { while (1) { } }
  void UsageFault_Handler(void) { while (1) { } }
}

int16_t convert_float_to_int16_t(float *output, size_t size) {
  int32_t tmp;
  for (int n = 0; n < size; n++) {
    tmp = (int32_t)(output[n] * 32768);
    tmp = (tmp <= -32768) ? -32768 : (tmp >= 32767) ? 32767 : tmp;
  }
  return (int16_t)tmp;
}

void FillBuffer(AudioDac::Frame* buf, size_t size) {
  mygreatdsp.compute(size, NULL, output);
  buf->l = convert_float_to_int16_t(output[0], size);
  buf->r = convert_float_to_int16_t(output[1], size);
}

int main(void) {
  mygreatdsp.init(kSampleRate);
  audio_dac.Init(int(kSampleRate), kBlockSize);
  audio_dac.Start(&FillBuffer);
  while (1) { }
}
