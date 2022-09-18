#include "ledGame.h"
#include "drivers/audio_dac.h"
using namespace std;
using namespace ledGame;

AudioDac audio_dac;

bool playing = false;

extern "C" {
  // Cortex-M4 Processor Exceptions Handlers:
  void NMI_Handler(void) { }
  void HardFault_Handler(void) { while (1) { } }
  void MemManage_Handler(void) { while (1) { } }
  void BusFault_Handler(void) { while (1) { } }
  void UsageFault_Handler(void) { while (1) { } }
}

void FillBuffer(AudioDac::Frame* output, size_t size) {
  // nothing yet
}

int main(void) {
  audio_dac.Init(int(kSampleRate), kBlockSize);
  audio_dac.Start(&FillBuffer); // send address here or?
  while (1) { }
}
