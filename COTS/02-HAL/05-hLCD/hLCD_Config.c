#include "STD_TYPES.h"

#include "GPIO.h"
#include "HLCD.h"
#include "HLCD_cfg.h"

#define Conc(b7, b6, b5, b4, b3, b2, b1, b0)		Conc_Help(b7, b6, b5, b4, b3, b2, b1, b0)
#define Conc_Help(b7, b6, b5, b4, b3, b2, b1, b0)	0b##b7##b6##b5##b4##b3##b2##b1##b0

#define INIT_STATE_1	Conc(0, 0, 1, 1, HLCD_NUMBER_OF_LINES, HLCD_FONT, 0, 0)
#define INIT_STATE_2	Conc(0, 0, 0, 0, 1, HLCD_DISPLAY, HLCD_CURSOR, HLCD_CURSOR_BLINK)
#define INIT_STATE_3	Conc(0, 0, 0, 0, 0, 0, 0, 1)
#define INIT_STATE_4	Conc(0, 0, 0, 0, 0, 1, HLCD_INC_DEC, HLCD_SHIFT)

extern const HLCD_cfgInitStateStruct_t HLCD_cfgInitStateStruct =
{
	.cfgInitState1 = INIT_STATE_1,
	.cfgInitState2 = INIT_STATE_2,
	.cfgInitState3 = INIT_STATE_3,
	.cfgInitState4 = INIT_STATE_4
};


extern const HLCD_cfgStruct_t HLCD_cfgStruct =
{
	.mode = HLCD_DATA_MODE,
	
    .dataPorts 	= {HLCD_D0_PORT, HLCD_D1_PORT, HLCD_D2_PORT, HLCD_D3_PORT,
					HLCD_D4_PORT, HLCD_D5_PORT, HLCD_D6_PORT, HLCD_D7_PORT},
    
	.dataPins 	= {HLCD_D0_PIN, HLCD_D1_PIN, HLCD_D2_PIN, HLCD_D3_PIN,
					HLCD_D4_PIN, HLCD_D5_PIN, HLCD_D6_PIN, HLCD_D7_PIN},
    
    .RS_Port 	= HLCD_RS_PORT,
    .RS_Pin 	= HLCD_RS_PIN,
    
    .RW_Port 	= HLCD_RW_PORT,
    .RW_Pin 	= HLCD_RW_PIN,
    
	.E_Port 	= HLCD_E_PORT,
	.E_Pin 		= HLCD_E_PIN
};
