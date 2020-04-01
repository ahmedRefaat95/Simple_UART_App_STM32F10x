/*********************************************************************
* Author: May Alaa                                                   *
* Version: v03                                                       *
* Date: 28/3/2020                                                    *
* Component: LED                                            		 *
* Layer: HAL                                                         *
* File Name: hLED_Cfg.c                                              *
**********************************************************************/

#include "STD_TYPES.h"
#include "GPIO_interface.h"
#include "hLED.h"

GPIO_t LED_GPIOCfg[LED_NUMBER_OF_LEDS] =
{
	    {
				.port  = PORTA,
				.pin   = PIN0,
				.mode  = OUTPUT_PUSH_PULL,
				.speed = OUTPUT_SPEED_2M
	    }
};

led_t LED_leds[LED_NUMBER_OF_LEDS] =
{
		{
				.GPIO = &LED_GPIOCfg,
				.ActiveState = LED_ACTIVE_HIGH
		}
};
