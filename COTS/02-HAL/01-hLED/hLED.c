/*********************************************************************
* Author: May Alaa                                                   *
* Version: v03                                                       *
* Date: 28/3/2020                                                    *
* Component: LED                                            		 *
* Layer: HAL                                                         *
* File Name: hLED.c                                                  *
**********************************************************************/


#include "STD_TYPES.h"
#include "GPIO_interface.h"
#include "RCC_interface.h"
#include "hLED.h"


extern const led_t LED_leds[LED_NUMBER_OF_LEDS];

/**************************************************************
 * Function:  hLED_Init 
 * --------------------
 *  Initializes GPIOs for the LEDs
 * 
 *  
 *  returns: void
***************************************************************/
void hLED_Init(void)
{
    u8 i;
    for(i=0; i<LED_NUMBER_OF_LEDS; i++)
    {   
        if(PORTA == LED_leds[i].GPIO[i]->port)
        {
            RCC_ControlPeripheralClock(APB2, APB2_IOPA, ON);
        }
        else if (PORTB == LED_leds[i].GPIO[i]->port)
        {
            RCC_ControlPeripheralClock(APB2, APB2_IOPB, ON);
        }
        else if (PORTC == LED_leds[i].GPIO[i]->port)
        {
            RCC_ControlPeripheralClock(APB2, APB2_IOPC, ON);
        }

        GPIO_Configure(LED_leds[i].GPIO[i]);
    } 
    
}

/**********************************************************************
 * Function:  hLED_ON 
 * --------------------
 *  Sets the Led on
 * 
 *  LED_Ch: The name of the LED
 *  
 *  returns: A status
 *                 OK : if the function is executed correctly
 *                 NOT_OK : if the function is not executed correctly
 ************************************************************************/
STD_ERROR hLED_ON(u8 LED_Ch)
{
    u8 Local_ErrSt = OK;
    if (NULL == LED_leds[LED_Ch].GPIO[LED_Ch])
    {
        Local_ErrSt = NOT_OK;
    }
    else
    {
        GPIO_WritePinVal(LED_leds[LED_Ch].GPIO[LED_Ch]->port, LED_leds[LED_Ch].GPIO[LED_Ch]->pin, LED_leds[LED_Ch].ActiveState);
    }
    return Local_ErrSt;
}

/**********************************************************************************************
 * Function:  Led_SetLedOff 
 * --------------------
 *  Sets the Led off
 * 
 *  LED_Ch: The name of the LED
 *  
 *  returns: A status
 *                 OK : if the function is executed correctly
 *                 NOT_OK : if the function is not executed correctly
 *********************************************************************************************/
STD_ERROR hLED_OFF(u8 LED_Ch)
{
    u8 Local_ErrSt = OK;
    if (NULL == LED_leds[LED_Ch].GPIO[LED_Ch])
    {
        Local_ErrSt = NOT_OK;
    }
    else
    {
        GPIO_WritePinVal(LED_leds[LED_Ch].GPIO[LED_Ch]->port, LED_leds[LED_Ch].GPIO[LED_Ch]->pin, !LED_leds[LED_Ch].ActiveState);
    }
    return Local_ErrSt ;
}

