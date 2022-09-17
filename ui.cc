// UI and CV processing ("controller" and "view")

#include "ui.h"

namespace ledGame {

using namespace std;

void Ui::Init() {
  Init_RNG();
  userButton.Init();
  leds.Init();
  Reset();
}

void Ui::Init_RNG()
{
  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
  RNG_Cmd(ENABLE);
}

void Ui::Reset() {
  while(RNG_GetFlagStatus(RNG_FLAG_DRDY)== RESET) { }; // essential
  rotateClockwise = (RNG_GetRandomNumber() % 2);
  if (rotateClockwise) {
    currentLed = 1;
    for (int i=0;i<4;i++) {
      leds.initialLedState[i] = loseRoundArray[i];
    }
  } else {
    currentLed = 2;
    for (int i=0;i<4;i++) {
      leds.initialLedState[i] = winRoundArray[i];
    }
  };
  gameState = IN_PROGRESS;
  leds.Reset();
}

void Ui::Poll() {
  leds.Update();
  switch (gameState) {
    case IN_PROGRESS:
    {
      RotateLEDs();
      PollUserButton();
    }
    break;
    case WIN_ROUND:
    {
      FlashWinLose(10, winRoundArray);
    }
    break;
    case WIN_GAME:
    {
      FlashWinLose(15, winGameArray);
    }
    break;
    case LOSE_GAME:
    {
      FlashWinLose(10, loseRoundArray);
    }
    break;
  }
  tickCount++; // any problem if this goes at end? yes, 1000 would not be start position
}

void Ui::PollUserButton() {
  if (userButton.pressed) {
    if (currentLed == targetLed) {
      if (currentRound < maxNoOfRounds) {
        gameState = WIN_ROUND;
        tickInterval = tickInterval * 0.8;
        tickCount = 0;
      }
      else
      {
        gameState = WIN_GAME;
        tickInterval = INIT_TICK_INTERVAL;
        currentRound = 0;
        tickCount = 0;
      }
    }
    else
    {
      gameState = LOSE_GAME;
      tickInterval = INIT_TICK_INTERVAL;
      currentRound = 0;
      tickCount = 0;
    }
  }
}

void Ui::RotateLEDs() {
  if ((tickCount % tickInterval) == tickInterval-1) {
    bool newLedArray[4];
    // rotate array:
    for (int i=0;i<4;i++) {
      if (rotateClockwise) {
        int j = ((i-1)+4)%4;
        newLedArray[i] = leds.ledArray[j];
      }
      else
      {
        int j = (i+1)%4;
        newLedArray[i] = leds.ledArray[j];
      }
    };
    // reassign array:
    for (int i=0;i<4;i++) {
      leds.ledArray[i] = newLedArray[i];
    };
    if (rotateClockwise) {targetLed = 0; currentLed++;} else {targetLed = 3; currentLed--;};
    if (currentLed == -1) {currentLed = 3;}; // hacky!
    currentLed = currentLed % 4;
  };
}

void Ui::FlashWinLose(int maxNoOfFlashes, bool arrayToAssign[]) {
  // flash ten times, twice as fast each time
  // TODO: set number of flashes to 10 here and count down
  if (noOfFlashes < maxNoOfFlashes) {
    int flashTickOffset = flashTickInterval/2;
    if ((tickCount % flashTickInterval) == flashTickInterval-1)
    {
      // this happens first
      leds.AssignArray(arrayToAssign);
    }
    if ((tickCount + flashTickOffset) % flashTickInterval == flashTickInterval-1)
    {
      // this happens second
      leds.AssignArray(emptyArray);
      flashTickInterval = flashTickInterval * 0.7;
      noOfFlashes++;
    }
  }
  else
  {
    noOfFlashes = 0;
    flashTickInterval = INIT_FLASHTICK_INTERVAL;
    currentRound++;
    Reset();
    userButton.pressed = false;
    tickCount = 0;
  }
}

} // namespace ledGame
