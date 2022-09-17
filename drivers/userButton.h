#include "../stmlib/stmlib.h" // DISALLOW_COPY_AND_ASSIGN
#include "stm32f4xx.h" // CMSIS Peripheral Access Layer (register defs, bit defs, memory mapping). This file prob needs to be included in every project
#include "stm32f4xx_rcc.h" // Reset and clock control (GPIO seems to run on clocks)
#include "stm32f4xx_gpio.h" // For working with General Purpose Input/Output pins
// #include <stm32f37x_conf.h> // the mutable way (WARNING - wrong board!)

#ifndef LEDGAME_USERBUTTON_H_
#define LEDGAME_USERBUTTON_H_

namespace ledGame {

class UserButton {
 public:
  UserButton() { }
  ~UserButton() { }

  void Init();

  //static volatile
  bool pressed = false;

  static UserButton* GetInstance() { return instance_; }

 private:
  void Init_GPIO();

  static UserButton* instance_;

  DISALLOW_COPY_AND_ASSIGN(UserButton);
};

}  // namespace ledGame

#endif  // LEDGAME_USERBUTTON_H_
