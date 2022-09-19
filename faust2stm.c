#include "drivers/audio_dac.h"
#include "noise.c"

static float output0[12] = {0.0};
static float output1[12] = {0.0};
static float* output[2] = {output0, output1};

// Cortex-M4 Processor Exceptions Handlers:
void NMI_Handler(void) { }
void HardFault_Handler(void) { while (1) { } }
void MemManage_Handler(void) { while (1) { } }
void BusFault_Handler(void) { while (1) { } }
void UsageFault_Handler(void) { while (1) { } }

Frame* convert_float_to_int16_t(Frame* buf, float **output, size_t size) {
  int32_t tmp;
  for (int c = 0; c < 2; c++) {
    for (int n = 0; n < size; n++) {
      tmp = (int32_t)(output[0][n] * 32768); // hack for mono - copy channel 0 twice
      tmp = (tmp <= -32768) ? -32768 : (tmp >= 32767) ? 32767 : tmp;
    }
    switch (c) {
      case 0:
        buf->l = (int16_t)tmp;
      case 1:
        buf->r = (int16_t)tmp;
      }
  }
  return buf;
}

void FillBuffer(Frame* buf, size_t size) {
  computemydsp(mydsp, size, NULL, output);
  buf = convert_float_to_int16_t(buf, output, size);
}

int main(void) {
  //newmydsp();
  initmydsp(mydsp, kSampleRate);
  Audio_DAC_Init(int(kSampleRate), kBlockSize);
  Audio_DAC_Start(&FillBuffer);
  while (1) { }
}