/*********************************************************************
* Author: May Alaa                                                   *
* Version: v03                                                       *
* Date: 28/3/2020                                                    *
* Component: SWitch                                            		 *
* Layer: HAL                                                         *
* File Name: hSwitch_Cfg.c                                           *
**********************************************************************/
#include "STD_TYPES.h"
#include "GPIO_interface.h"
#include "hSwitch.h"

GPIO_t Switch_GPIOCfg[SWITCH_NUMBER_OF_SWITCHES] =
{
		{
			.port  = PORTA,
			.pin   = PIN7,
			.mode  = INPUT_PULLUP_DOWN,
			.speed = INPUT_SPEED
		}
};

switch_t Switch_switches[SWITCH_NUMBER_OF_SWITCHES] =
{
		{
			.GPIO = Switch_GPIOCfg,
			.activeState = SWITCH_PULL_UP
		}
};
