#include "../stmlib/stmlib.h" // DISALLOW_COPY_AND_ASSIGN
#include "stm32f4xx.h" // CMSIS Peripheral Access Layer (register defs, bit defs, memory mapping). This file prob needs to be included in every project
#include "stm32f4xx_rcc.h" // Reset and clock control (GPIO seems to run on clocks)
#include "stm32f4xx_gpio.h" // For working with General Purpose Input/Output pins
// #include <stm32f37x_conf.h> // the mutable way (WARNING - wrong board!)

#ifndef LEDGAME_LEDS_H_
#define LEDGAME_LEDS_H_

namespace ledGame {

class Leds {
 public:
  Leds() { }
  ~Leds() { }

  void Init();
  void Reset();
  void Update();
  void AssignArray(bool arrayToAssign[]);
//  void SysTick_Init();

  bool ledArray[4];
  bool initialLedState[4] = {1,0,0,0};

 private:
   void Init_GPIO();

  enum GameState {
    IN_PROGRESS,
    WIN_ROUND,
    WIN_GAME,
    LOSE_GAME
  };

  GameState gameState_;

  // static const
  uint16_t ledPins[4] { GPIO_Pin_13, GPIO_Pin_14, GPIO_Pin_15, GPIO_Pin_12 };


  DISALLOW_COPY_AND_ASSIGN(Leds);
};

}  // namespace ledGame

#endif  // LEDGAME_LEDS_H_
