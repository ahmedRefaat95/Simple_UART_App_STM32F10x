/*********************************************************************
* Author: May Alaa                                                   *
* Version: v03                                                       *
* Date: 28/3/2020                                                    *
* Component: SWitch                                            		 *
* Layer: HAL                                                         *
* File Name: hSwitch.c                                               *
**********************************************************************/

#include "STD_TYPES.h"
#include "GPIO_interface.h"
#include "RCC_interface.h"
#include "hSwitch.h"

u8 Switch_Status[SWITCH_NUMBER_OF_SWITCHES];

extern const switch_t Switch_switches[SWITCH_NUMBER_OF_SWITCHES];
/*****************************************************************
 * Function: hSwitch_Init
 * --------------------
 *  Initializes GPIOs for the Switches
 * 
 *  returns: void
 *                 
 *****************************************************************/
void hSwitch_Init(void)
{
    u8 i;
    for(i=0; i<SWITCH_NUMBER_OF_SWITCHES; i++)
    {
        if(PORTA == Switch_switches[i].GPIO[i]->port)
        {
            RCC_ControlPeripheralClock(APB2, APB2_IOPA, ON);
        }
        else if (PORTB == Switch_switches[i].GPIO[i]->port)
        {
            RCC_ControlPeripheralClock(APB2, APB2_IOPB, ON);
        }
        else if (PORTC == Switch_switches[i].GPIO[i]->port)
        {
            RCC_ControlPeripheralClock(APB2, APB2_IOPC, ON);
        }

        GPIO_Configure(Switch_switches[i].GPIO[i]);

        if(Switch_switches[i].activeState == SWITCH_PULL_UP)
        {
        	GPIO_WritePinVal(Switch_switches[i].GPIO[i]->port, Switch_switches[i].GPIO[i]->pin,1);
        }
    }

}


/**************************************************************************
 * Function:  hSwitch_Read                                                
 * --------------------
 *  Gets the status of the switch
 * 
 *  Switch_Ch: The name of the Switch ex: SWITCH_COUNTERS
 *              denotes the index of the switch in the array
 *                   
 *  state: Save the status of the switch in
 *                 SWITCH_PRESSED : if the switch is pressed
 *                 SWITCH_NOT_PRESSED : if the switch is not pressed
 *  returns: A status
 *                 OK : if the function is executed correctly
 *                 NOT_OK : if the function is not executed correctly
 ***************************************************************************/
STD_ERROR hSwitch_ReadStatus(u8 Switch_Ch, u8* reading)
{
    u8 Local_read=0;
    u8 Local_ErrStat =OK;
	if(NULL==reading)
	{
		Local_ErrStat = NOT_OK;
	}
	else
	{
		*reading = Switch_Status[Switch_Ch];
	}
    return Local_ErrStat;
}

void hSwitch_DebounceTask(void)
{
	u8 i=0, Current_Status;
	static u8 Prev_Status=0, Switch_debCount=0;
	for(i=0;i<SWITCH_NUMBER_OF_SWITCHES; i++)
	{
		GPIO_GetPinVal(Switch_switches[Switch_Ch].GPIO[Switch_Ch]->port,Switch_switches[Switch_Ch].GPIO[Switch_Ch]->pin, &Current_Status);
		if(Prev_Status == Current_Status)
		{
			Switch_debCount++;
		}
		else 
		{
			Switch_debCount=0;
		}
		if(SWITCH_DEB_EDGES == Switch_debCount)
		{
			Switch_Status[i] = Current_Status;
			Switch_debCount  = 0;
		}
		Prev_Status = Current_Status;
	}
}
