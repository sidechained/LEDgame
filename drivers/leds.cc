// Driver for 4 coloured LED's of STM32F4 Discovery Board:
// User LD3: TOP orange LED is a user LED connected to the I/O PD13 of the STM32F407VGT6.
// User LD5: RIGHT red LED is a user LED connected to the I/O PD14 of the STM32F407VGT6.
// User LD6: BOTTOM blue LED is a user LED connected to the I/O PD15 of the STM32F407VGT6.
// User LD4: LEFT green LED is a user LED connected to the I/O PD12 of the STM32F407VGT6.

#include "leds.h"

void Leds::Init() {
  Init_GPIO();
}

void Leds::Reset() {
  AssignArray(initialLedState);
}

void Leds::Init_GPIO() {
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  GPIO_InitTypeDef gpio_init;
  gpio_init.GPIO_Mode = GPIO_Mode_OUT;
  gpio_init.GPIO_OType = GPIO_OType_PP;
  gpio_init.GPIO_Speed = GPIO_Speed_100MHz;
  gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
  gpio_init.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_Init(GPIOD, &gpio_init);
}

void Leds::Update() {
  for (int i = 0; i < 4; i++) { // i < 4?
    if (ledArray[i]) {
      GPIO_SetBits(GPIOD, ledPins[i]);
    }
    else
    {
      GPIO_ResetBits(GPIOD, ledPins[i]);
    }
  }
}

void Leds::AssignArray(bool arrayToAssign[])
{
  for (int i = 0; i < 4; i++) {
    ledArray[i] = arrayToAssign[i];
  }
}