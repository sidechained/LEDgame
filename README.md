# ToDo list:
 
- fix high-pitched whine on flash
- fix makefile errors i.e. warning: _kill is not implemented and will always fail
-- see https://stackoverflow.com/questions/61098390/i-cant-compile-with-arm-none-eabi-gcc
- tidy up makefile and headers (make sure only files used are referenced)
- fork mutable eurorack modules and pare down to what we need (inc resources)

- first without venv, then with it
- first without openocd debug, then with (where is DB?)
- first on mac then linux

# Introduction

This is a simple project to create a flashing LED game with real-time sound for the STM32F4 Discovery Board. It was written as a 'bare bones' test project to test sound and peripherals working together.

The sound synthesis code is based on Mutable Instruments STM32F4 based eurorack modules [https://github.com/pichenettes/eurorack]. It turns out a lot more work was needed than expected to get this code running on the discovery board and outside of the mutable instrument development environment

This was a lot to do with my lack of experience.
as the specs/drivers/peripherals are somewhat esoteric.

I hope to be able build on this code to create more interesting synthesiser projects in the future.

# Setup

- Power the board (e.g. from the USB mini B port)
- Connect a sound source to the headphone jack
- Place the discovery board with the headphone jack pointing towards you

# How the Game Works

- The orange, red, blue and green LED's will flash clockwise or counter-clockwise in sequence
- The aim of the game is press the blue 'user' button when the led is on orange (i.e. at the top)
- When successful the green LED on the left side will flash and a 'win-round' sound will play
- When unsuccesful the red LED on the right side will flash and a 'lose-round' sound will play
- After each round the speed of the sequence will get faster and it's direction can change
- The game is won after 7 rounds, where a 'win-game' sound will play and the game resets
- Pressing the black 'reset' button will also restart the game

# Dependencies

## Create a virtual environment (optional)
This avoids version conflicts with already globally installed packages.

(but unfortunately homebrew can't be installed within it, and neither can the arm gnu toolchain packages) - these are challenges for later

$ python3 -m venv ledGameenv
$ source ledGameenv/bin/activate

## Install Arm GNU toolchain
- packages here: https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads
- download package for mac for 'bare-metal target' (arm-none-eabi)
- when installed, it ends up in /Applications
- ideally we would install this in our virtual environment also, going with this approach for now

## Install ST-link Utility
- we will use homebrew to install an open-source version of the STM STLink Tools
- these are used to flash binaries to the board
- see https://github.com/stlink-org/stlink

$ /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
$ brew install stlink

## Install Open-OCD (optional)
- open-ocd is used to do debugging on the board
- if this is not wanted please

$ brew install open-ocd

## Install STM32F4-Discovery firmware package
Download STSW-STM32068 from (here)[https://www.st.com/en/evaluation-tools/stm32f4discovery.html#tools-software]
Unzip STM32F4-Discovery_FW_V1.1.0 and place in the /stm directory

## Install STM32F4xx standard peripherals library
- Low level drivers covering the peripherals, belonging to the STM32F4 series
- complete register address mapping with all bits, bitfields and registers declared in C
Download STSW-STM32065 1.8.0 from the dropdown menu (here)[https://www.st.com/en/embedded-software/stsw-stm32065.html]
Unzip STM32F4xx_DSP_StdPeriph_Lib_V1.8.0 and place in the /stm directory

## Install stmlib

# Build

The makefile handles the build and can be build from

'make' 

To tidy up 'make clean')

# Flash

To flash via st-link over USB, ensure you have a USB cable connected from your computer to the USB mini B port on the board.

'make flash'. 

This will erase the flash memory first, which unfortunately can take some time. In my experience the flash is not always reliable before the erase.

# Understanding The Code

## Main Code (root dir)

ledGame.cc/h
- the 'main' of the project
- everything takes place within the DAC fill buffer loop
- ie. polling ui, checking gamestate and triggering sounds

resources.cc/h
- this file taken from /mutable-dev-environment/eurorack-modules/plaits/
- some hardcoded wave data - which functions are used? presumably sine ones

sounds.cc/h
- core Sound functions (decay envelope to l+r outputs)
- winSound: standard 'oscillator' function, triggering, rendering wingame and winround sounds
- loseSound: 'harmonic oscillator' function, triggering, rendering

ui.cc/h
- audio and CV processing ("controller" and "view")
- ui in this case means the user button and LEDs
- Functions to PollUserButton, RotateLEDs and FlashWinLose
- Poll function handles four gamestates IN_PROGRESS, WIN_ROUND, WIN_GAME, LOSE_GAME

## Drivers (/drivers)

audio_dac.cc/h
- driver to configure CS43L22 Audio Codec, setup DMA and IS2 for streaming

leds.cc/h
- driver for the four coloured LED's (Q: can systick parts be removed?)

userButton.cc/h
- driver for the blue user button

## DSP (/dsp)
DSP code used to make the sounds (taken from Mutable Instruments' Plaits)

dsp.h
- sets sample rate, corrected sample rate, a0 freq, audio blocksizes
- defined as LEDGAME_DSP_DSP_H_ (so not a direct copy from mutable)
- but should be able to copy directly

envelope.h
- Envelope for the internal LPG, plus a decay envelope (namespace ledGame)

fx/low_pass_gate.h
- basis of our loseSound

oscillator/harmonic_oscillator.h
- basis of our loseSound

oscillator/oscillator.h
- basis of our winSounds

voice.h
- a voice is an instance of ChannelPostProcessor, consisting of a limiter, LPG and clipper

## stmlib
General purpose DSP code that the higher level code uses (taken from the general purpose Mutable Instruments stmlib)

stmlib.h
- some basic definitions used in stmlib i.e. define CLIP, CONSTRAIN etc
- also: DISALLOW_COPY_AND_ASSIGN

dsp/dsp.h
- DSP utility routines - what is being used here?

dsp/filter.h
- Zero-delay-feedback filters (one pole and SVF)

dsp/limiter.h
- Limiter

dsp/parameter_interpolator.h
- Linear interpolation of parameters in rendering loops

dsp/polyblep.h
- Polynomial approximation of band-limited step for band-limited waveform synthesis

## STM Libraries (/stm)

### From the standard peripheral library (STM32F4xx_DSP_StdPeriph_Lib_V1.8.0) we are using:

startup_stm32f40_41xxx.s
- STM32F40xxx/41xxx Devices vector table for Atollic TrueSTUDIO toolchain (source code file written in assembly)
- compiled along with other source .cc files
- found in /Libraries/CMSIS/Device/ST/STM32F4xx/Source/Templates/TrueSTUDIO/

STM32F417IG_FLASH.ld
- linker script that works with our board
- is specified with the g++ compiler flag -T
- found in STM32F4xx_DSP_StdPeriph_Lib_V1.8.0/Project/STM32F4xx_StdPeriph_Templates/TrueSTUDIO/STM32F40_41xxx/

### From the discovery board library (STM32F4-Discovery_FW_V1.1.0) we are using:

misc.c # for NVIC
stm32f4xx_dma.c
stm32f4xx_exti.c
stm32f4xx_gpio.c
stm32f4xx_i2c.c
stm32f4xx_rcc.c
stm32f4xx_rng.c
stm32f4xx_spi.c
stm32f4xx_syscfg.c
- functions to interface with peripherals on the board
- found in /Libraries/STM32F4xx_StdPeriph_Driver/src/

stm32f4_discovery.c/h
- firmware functions to manage LEDs and push-button
- found in /Utilities/STM32F4-Discovery/

stm32f4xx_conf.h
- TODO: what does this do?
- Uncomment lines to enable peripheral header file inclusion (everything seems commented!)
- found in /Project/Audio_playback_and_record/inc/

system_stm32f4xx.c
- system clock configuration for STM32F4xx devices (CSMIS file)
- found in /Project/Audio_playback_and_record/src/

<!-- stm/STM32F4-Discovery_FW_V1.1.0/Project/Audio_playback_and_record/inc/stm32f4xx_it.h
- contains the headers of the interrupt handlers (exception handlers) -->