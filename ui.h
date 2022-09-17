#ifndef LEDGAME_UI_H_
#define LEDGAME_UI_H_

#include <stdlib.h> // rand
#include <time.h> // rand seed

#include "drivers/leds.h"
#include "drivers/userButton.h"

namespace ledGame {

class Ui {
 public:
  Ui() { }
  ~Ui() { }

  void Init();
  void Poll();

  enum GameState {
    IN_PROGRESS,
    WIN_ROUND,
    WIN_GAME,
    LOSE_GAME
  };

  GameState gameState;

 private:
  void Init_RNG();
  void Reset();
  void PollUserButton();
  void RotateLEDs();
  void FlashWinLose(int maxNoOfFlashes, bool arrayToAssign[]);

  Leds leds;
  UserButton userButton;

  bool rotateClockwise;
  #define INIT_FLASHTICK_INTERVAL 2500;
  int flashTickInterval = INIT_FLASHTICK_INTERVAL;
  int noOfFlashes = 0;

  bool winRoundArray[4] = {0,0,0,1}; // 3 LEFT GREEN for win round
  bool loseRoundArray[4] = {0,1,0,0}; // 1 RIGHT RED for lose round
  bool winGameArray[4] = {1,1,1,1}; // ALL ON for win game
  bool emptyArray[4] = {0,0,0,0}; // when turning all LEDS off

  #define INIT_TICK_INTERVAL 5000;
  int tickInterval = INIT_TICK_INTERVAL;
  int tickCount = 0;
  int currentLed;
  int targetLed = 0;
  int currentRound = 0;
  int maxNoOfRounds = 7;

  // for enveloping:
  float phase;
  int tickDur;
  float decPnt;
  float susPnt;
  float susLev;
  float relPnt;
  int tick = 0;
  bool playing = false;

  DISALLOW_COPY_AND_ASSIGN(Ui);
};

}  // namespace ledGame

#endif  // LEDGAME_UI_H_
