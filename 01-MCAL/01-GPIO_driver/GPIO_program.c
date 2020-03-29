/************************************************************************
 * File name: GPIO.c
 *
 * Description: This file contains the implementation of the GPIO driver
 * functions
 *
 * owner: Ahmed Refaat
 * date: 6/3/2020
 * version 1.2
 ***************************************************************************/
#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "GPIO_interface.h"

/* This API shall configure the GPIO pins ( single pin or a whole port )
 *
 * Inputs :
 * 1- Address of the configuration structure
 */
STD_ERROR GPIO_Configure(GPIO_t * cfg_ptr) {
	STD_ERROR LocError = NOT_OK;
	u8 tmpPin = 0;
	u32 TempRegisterValue;
	u16 Iterator = 0;

	if ( cfg_ptr != NULL)
	{
		LocError = OK;
		for (Iterator = 0; Iterator < 16; Iterator++) {
			if( ( (cfg_ptr->pin) >> Iterator )& 0x01 )
			{
				if (Iterator <= 7) {

					/* Clear 4 bits of mode configuration*/
					TempRegisterValue =  (cfg_ptr-> port -> GPIO_CRL) & ~(CLR_MODE_BITS << (Iterator * 4));
					/* Set the mode 4 bits */
					TempRegisterValue |= (cfg_ptr->mode << (Iterator * 4));
					TempRegisterValue |= (cfg_ptr->speed << (Iterator * 4));

					cfg_ptr-> port -> GPIO_CRL = TempRegisterValue;
				}
				else if (Iterator <= 15)
				{
					tmpPin = Iterator - 8;
					/* Clear 4 bits of mode configuration*/
					TempRegisterValue = (cfg_ptr->port->GPIO_CRH) & ~(CLR_MODE_BITS << (tmpPin * 4));
					/* Set the mode 4 bits */
					TempRegisterValue|= (cfg_ptr->mode << (tmpPin * 4));
					TempRegisterValue |= (cfg_ptr->speed << (tmpPin * 4));

					cfg_ptr->port->GPIO_CRH = TempRegisterValue;
				}
			}
			else
			{
				//The pin does not need to be configured
			}
		}

	}
	else
	{
		LocError = NOT_OK;
	}

	return LocError;
}


/* This API shall write a value on the GPIO pins
 * Inputs :
 * 1- PORT address , ranges are :
 * 		1- PORTA
 * 		2- PORTB
 * 		3- PORTC
 * 2 - Pin number , ranges are :
 * 		PINx ( 1 ... 15 )
 * 3 - Pin value , ranges are :
 * 		1- HIGH
 * 		2- LOW
 */
STD_ERROR GPIO_WritePinVal(GPIO_registers* Port, u32 Pin, u8 Val) {
	STD_ERROR LocError = NOT_OK;

	if ( ( Port != NULL) && CHECK_PIN(Pin) && CHECK_PORT(Port) )
	{
		LocError = OK;

		if (Val == HIGH)
		{
			Port->GPIO_BSRR = Pin;
		}
		else if (Val == LOW)
		{
			Port->GPIO_BRR =Pin;
		}
		else
		{
			LocError = NOT_OK;
		}

	}
	else
	{
		LocError = NOT_OK;
	}

	return LocError;
}

/* This API shall get the pin value of desired GPIO pins
 * Inputs :
 * 1- PORT address , ranges are :
 * 		1- PORTA
 * 		2- PORTB
 * 		3- PORTC
 * 2 - Pin number , ranges are :
 * 		PINx ( 1 ... 15 )
 * Outputs : The value of desired pin of type u8
 */
STD_ERROR GPIO_GetPinVal(GPIO_registers* Port, u32 Pin ,u8* Data) {
	STD_ERROR LocError = NOT_OK;

	if ( ( Port != NULL) && CHECK_PIN(Pin) && CHECK_PORT(Port) )
	{
		LocError = OK;
		if (Port->GPIO_IDR & Pin) {
			*Data = 1;
		} else {
			*Data = 0;
		}

	}
	else
	{
		LocError = NOT_OK;
	}

	return LocError;
}
