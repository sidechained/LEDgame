// Author: Xavier Halgand & Graham Booth

#ifndef NANO2_MIDI_DRIVER_H_
#define NANO2_MIDI_DRIVER_H_

/* Includes ------------------------------------------------------------------*/

#include "stdio.h"
#include "usbh_core.h"
#include "usbh_MIDI.h"

#include <math.h>
#include <stdint.h>
#include <stdbool.h>
/* Includes ------------------------------------------------------------------*/

// #include <math.h>
// #include <stdlib.h>
// #include <stdio.h>
// #include <stdbool.h>

// /* Local includes ------------------------------------------------------------------*/
// #include "stm32f4xx_hal.h"
// #include "stm32f4_discovery.h"
// #include "stm32f4_discovery_audio.h"
// #include "stm32f4_discovery_accelerometer.h"
// #include <stdio.h>
// #include "stm32f4xx_it.h"
// #include "lis302dl.h"

/*------------------------------------------------------------------------------*/
typedef enum
{
	APPLICATION_IDLE = 0,
	APPLICATION_START,
	APPLICATION_READY,
	APPLICATION_RUNNING,
	APPLICATION_DISCONNECT
}
MIDI_ApplicationTypeDef;

extern USBH_HandleTypeDef hUSBHost;
extern MIDI_ApplicationTypeDef Appli_state;
extern int8_t currentNote;
extern int8_t velocity;

//void Reset_notes_On(void);
void USB_MIDI_Init(void);
static void USBH_UserProcess_callback(USBH_HandleTypeDef *pHost, uint8_t vId);
void MIDI_Application(void);
void Error_Handler(void);

#endif /* NANO2_MIDI_DRIVER_H_ */
