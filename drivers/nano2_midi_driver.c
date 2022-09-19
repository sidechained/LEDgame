// Author: Xavier Halgand & Graham Booth

#include "nano2_midi_driver.h"

#define RX_BUFF_SIZE 64 /* USB MIDI buffer : max received data 64 bytes */

uint8_t MIDI_RX_Buffer[RX_BUFF_SIZE]; // MIDI reception buffer

int8_t currentNote;
int8_t velocity;
uint8_t notes_On[128] = {0};
int8_t notesCount = 0; // number of notes on (keys pressed)

USBH_HandleTypeDef hUSBHost;
MIDI_ApplicationTypeDef Appli_state = APPLICATION_IDLE;

/* Private function prototypes -----------------------------------------------*/
void ProcessReceivedMidiDatas(void);

void USB_MIDI_Init(void)
{
	// called from main after all the audio stuff:

	/*## Init Host Library ################################################*/
	USBH_Init(&hUSBHost, USBH_UserProcess_callback, 0);

	/*## Add Supported Class ##############################################*/
	USBH_RegisterClass(&hUSBHost, USBH_MIDI_CLASS);

	/*## Start Host Process ###############################################*/
	USBH_Start(&hUSBHost);

	while (1)
	{
		MIDI_Application();

		/* USBH_Background Process */
		USBH_Process(&hUSBHost);
	}
}

static void USBH_UserProcess_callback(USBH_HandleTypeDef *pHost, uint8_t vId)
{
	switch (vId)
	{
	case HOST_USER_SELECT_CONFIGURATION:
		break;

	case HOST_USER_DISCONNECTION:
		Appli_state = APPLICATION_DISCONNECT;
		// BSP_LED_Off(LED_Green);
		// BSP_LED_Off(LED_Blue);
		break;

	case HOST_USER_CLASS_ACTIVE:
		Appli_state = APPLICATION_READY;
		// BSP_LED_On(LED_Green);
		// BSP_LED_Off(LED_Blue);
		//BSP_LED_Off(LED_Red);
		break;

	case HOST_USER_CONNECTION:
		Appli_state = APPLICATION_START;
		// BSP_LED_On(LED_Blue);
		break;

	default:
		break;
	}
}

void Error_Handler(void)
{
	/* Turn LED5 on */
	//BSP_LED_On(LED_Red);
	while (1)
	{
	}
}

void MIDI_Application(void)
{
	if (Appli_state == APPLICATION_READY)
	{
		USBH_MIDI_Receive(&hUSBHost, MIDI_RX_Buffer, RX_BUFF_SIZE); // just once at the beginning, start the first reception
		Appli_state = APPLICATION_RUNNING;
	}
	if (Appli_state == APPLICATION_RUNNING)
	{
		//....pffff......grrrrr......
	}

	if (Appli_state == APPLICATION_DISCONNECT)
	{
		Appli_state = APPLICATION_IDLE;
		USBH_MIDI_Stop(&hUSBHost);
	}
}

void USBH_MIDI_ReceiveCallback(USBH_HandleTypeDef *phost)
{
	ProcessReceivedMidiDatas();
	USBH_MIDI_Receive(&hUSBHost, MIDI_RX_Buffer, RX_BUFF_SIZE); // start a new reception
}

// void Reset_notes_On(void)
// {
// 	notesCount = 0;
// 	for (uint8_t i = 0; i < 128; i++)
// 		notes_On[i] = 0;
// }

void ProcessReceivedMidiDatas(void)
{
	uint16_t numberOfPackets;
	uint8_t *ptr = MIDI_RX_Buffer;
	midi_package_t pack;

	numberOfPackets = USBH_MIDI_GetLastReceivedDataSize(&hUSBHost) / 4; //each USB midi package is 4 bytes long

	while (numberOfPackets--)
	{
		pack.cin_cable = *ptr;
		ptr++;
		pack.evnt0 = *ptr;
		ptr++;
		pack.evnt1 = *ptr;
		ptr++;
		pack.evnt2 = *ptr;
		ptr++;

		if (pack.cin_cable != 0) // if incoming midi message...
			start_LED_On(LED_Blue, 8);

		if ((pack.evnt0 & 0xF0) == 0x80) // Note off ? -------------------------------
		{
			uint8_t noteOff = pack.evnt1;
			//if (notes_On[noteOff] == 1) {
			notes_On[noteOff] = 0;
			notesCount--;
			if (notesCount <= 0) // no more keys pressed
			{
				// CALL NOTE_OFF FUNCTION HERE!
				notesCount = 0;
			}
			else // some keys still pressed... (legato)
			{
				if ((noteOff - LOWEST_NOTE) == currentNote) // then let sound the lowest key pressed
				{
					uint8_t i;
					for (i = 0; i < 128; i++)
					{
						if (notes_On[i] == 1) // find the lowest key pressed
							break;
					}
					currentNote = i - LOWEST_NOTE; // conversion for notesFreq[]
				}
			}
			//}
		}
		else if ((pack.evnt0 & 0xF0) == 0x90) // Note on ----------------------------
		{
			uint8_t noteOn = pack.evnt1;
			velocity = pack.evnt2;
			if (velocity > 0) // True note on !
			{
				if (noteOn < LOWEST_NOTE) // filtering notes
				{
					currentNote = 0;
				}
				else
				{
					currentNote = noteOn - LOWEST_NOTE; // conversion for notesFreq[]
				}
				// CALL NOTE_ON FUNCTION HERE!
				notesCount++;
				notes_On[noteOn] = 1;
			}
			else
			{
				// ---------  Sometimes Note On with zero velocity means Note Off !
				notes_On[noteOn] = 0;
				notesCount--;
				if (notesCount <= 0)
				{
					// CALL NOTE_OFF FUNCTION HERE!
					notesCount = 0;
				}
				else
				{
					if ((noteOn - LOWEST_NOTE) == currentNote)
					{
						uint8_t i;
						for (i = 0; i < 128; i++)
						{
							if (notes_On[i] == 1) // find the lowest key pressed
								break;
						}
						currentNote = i - LOWEST_NOTE; // conversion for notesFreq[]
					}
				}
			}
		}
		else if ((pack.evnt0 & 0xF0) == 0xA0) // Aftertouch
		{
			// Filter1Res_set(pack.evnt2);
		}
		else if ((pack.evnt0 & 0xF0) == 0xE0) // Pitch Bend
		{
			// int16_t pitchBend = ((pack.evnt1 << 7) + pack.evnt2) - 0x2000;
		}

		/*------------------------------------------------------------------------------------------*/

		if ((pack.evnt0 & 0xF0) == 0xB0) /* If incoming midi message is a Control Change... */
		{
			uint8_t val = pack.evnt2;

			switch (pack.evnt1) // CC number
			{
			// FADERS 1-8
			case 0:
				// unmapped
				break;
			case 1:
				// unmapped
				break;
			case 2:
				// unmapped
				break;
			case 3:
				// unmapped
				break;
			case 4:
				// unmapped
				break;
			case 5:
				// unmapped
				break;
			case 6:
				// unmapped
				break;
			case 7:
				// unmapped
				break;

			// KNOBS 1-8
			case 16:
				// unmapped
				break;
			case 17:
				// unmapped
				break;
			case 18:
				// unmapped
				break;
			case 19:
				// unmapped
				break;
			case 20:
				// unmapped
				break;
			case 21:
				// unmapped
				break;
			case 22:
				// unmapped
				break;
			case 23:
				// unmapped
				break;

			// SOLO BUTTONS 1-8
			case 32:
				// unmapped
				break;
			case 33: 
				// unmapped
				break;
			case 34:
				// unmapped
				break;
			case 35:
				// unmapped
				break;
			case 36:
				// unmapped
				break;
			case 37:
				// unmapped
				break;
			case 38:
				// unmapped
				break;
			case 39:
				// unmapped
				break;

			// MUTE BUTTONS 1-8
			case 48:
				// unmapped
				break;
			case 49:
				// unmapped
				break;
			case 50:
				// unmapped
				break;
			case 51:
				// unmapped
				break;
			case 52:
				// unmapped
				break;
			case 53:
				// unmapped
				break;
			case 54:
				// unmapped
				break;
			case 55:
				// unmapped
				break;

			// // RECORD ARM BUTTONS 1-8 (commented out as these currently play notes 96-103)
			// case 64:
			// case 65:
			// case 66:
			// case 67:
			// case 68:
			// case 69:
			// case 70:
			// case 71:

			// LOWER TRANSPORT SECTION
			case 43: // Rewind Button
				// unmapped
				break;
			case 44: // Fast Forward Button
				// unmapped
				break;
			case 42: // Stop
				// unmapped
				break;
			case 41: // Play
				// unmapped
				break;
			case 45: // Record
				break;
			
			// UPPER TRANSPORT SECTION

			case 46: // Cycle 
				// unmapped
				break;
			case 58: // Track Decrement (<)
				// unmapped
				break;
			case 59: // Track Increment (>)
				// unmapped
				break;
			case 60: // Marker Set
				// unmapped
				break;
			case 61: // Marker Increment (<)
				// unmapped			octave down
				break;
			case 62: // Marker Decrement (>)
				// unmapped			octave up
				break;
			}
		}
	}
}