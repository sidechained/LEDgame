// TODO: how to use stm32f4xx_conf.h?
// TODO: calc time of an audio block (in relation to seconds) for accurate UI timing
// system_stm32f4xx.c CMSIS system clock info - can this be found in library? it can but it differs, see here:
// ./STM32F4xx_DSP_StdPeriph_Lib_V1.8.0/Libraries/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c

#include "ledGame.h"

using namespace std;
using namespace ledGame;
using namespace stmlib;

AudioDac audio_dac;
Ui ui;
WinSound winSound;
LoseSound loseSound;

// inline float Interpolate(const float* table, float index, float size);

enum GameState {
  IN_PROGRESS,
  WIN_ROUND,
  WIN_GAME,
  LOSE_GAME
};

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
  ui.Poll();
  switch (ui.gameState) {
    case IN_PROGRESS:
    {
      playing = false;
    }
    break;

    case WIN_ROUND:
    {
      if (!playing) {winSound.Trigger();};
      winSound.RenderWinRound(output, size);
      playing = true;
    }
    break;

    case WIN_GAME:
    {
      if (!playing) {winSound.Trigger();};
      winSound.RenderWinGame(output, size);
      playing = true;
    }
    break;

    case LOSE_GAME:
    {
      if (!playing) {loseSound.Trigger();};
      loseSound.Render(output, size);
      playing = true;
    }
    break;
  }
}

int main(void) {
  winSound.Init();
  loseSound.Init();
  ui.Init();
  audio_dac.Init(int(kSampleRate), kBlockSize);
  audio_dac.Start(&FillBuffer); // send address here or?
  while (1) { }
}
