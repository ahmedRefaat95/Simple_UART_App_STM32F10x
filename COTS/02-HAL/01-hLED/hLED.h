/*********************************************************************
* Author: May Alaa                                                   *
* Version: v03                                                       *
* Date: 28/3/2020                                                    *
* Component: LED                                            		 *
* Layer: HAL                                                         *
* File Name: hLED.h                                                  *
**********************************************************************/

#ifndef __HLED_H__
#define __HLED_H__

#define LED_ON              1
#define LED_OFF             0


#define LED_NUMBER_OF_LEDS        1
#define LED_INDICATION            0

#define LED_ACTIVE_LOW            0
#define LED_ACTIVE_HIGH           1


typedef struct
{
	GPIO_t * GPIO[ LED_NUMBER_OF_LEDS ];
    u8 ActiveState;
} led_t;

void hLED_Init (void);
STD_ERROR hLED_ON (u8 LED_Ch);
STD_ERROR hLED_OFF(u8 LED_Ch);


#endif
