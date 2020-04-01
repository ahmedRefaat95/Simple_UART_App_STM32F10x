/*********************************************************************
* Author: May Alaa                                                   *
* Version: v03                                                       *
* Date: 28/3/2020                                                    *
* Component: SWitch                                            		 *
* Layer: HAL                                                         *
* File Name: hSwitch.h                                               *
**********************************************************************/
#ifndef __HSWITCH_H__
#define __HSWITCH_H__

#define SWITCH_NUMBER_OF_SWITCHES           1

#define SWITCH_PULL_UP          			0
#define SWITCH_PULL_DOWN          			1
#define SWITCH_PRESSED                      1
#define SWITCH_NOT_PRESSED                  0

#define SWITCH_DEB_EDGES					5
#define SWITCH_COUNTERS						0



typedef struct
{
	GPIO_t * GPIO[SWITCH_NUMBER_OF_SWITCHES];
	uint8_t activeState;
}switch_t;


/*****************************************************************
 * Function: hSwitch_Init
 * --------------------
 *  Initializes GPIOs for the Switches
 *  returns: void
 *                 
 *****************************************************************/
void hSwitch_Init(void);


/**************************************************************************
 * Function:  hSwitch_ReadStatus
 * --------------------
 *  Gets the status of the switch
 * 
 *  Switch_Ch: The name of the Switch ex: SWITCH_COUNTERS
 *				denotes the index of the switch in the array
 *                   
 *  state: Save the status of the switch in
 *                 SWITCH_PRESSED : if the switch is pressed
 *                 SWITCH_NOT_PRESSED : if the switch is not pressed
 *  returns: A status
 *                 OK : if the function is executed correctly
 *                 NOT_OK : if the function is not executed correctly
 ***************************************************************************/
STD_ERROR hSwitch_ReadStatus(uint8_t Switch_Ch, uint8_t* reading);

#endif
