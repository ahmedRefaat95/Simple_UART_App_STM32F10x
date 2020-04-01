#include "STD_TYPES.h"

#include "GPIO.h"
#include "HLCD.h"
#include "SCHEDULER.h"

/* LCD Data types macros */
#define HLCD_CMD_TYPE	0
#define HLCD_DATA_TYPE	1

/* LCD commands macros */
#define HLCD_CLEAR_CMD		0x01

/* LCD init states */
typedef enum
{
	initS1,
	initS2,
	initS3,
	initS4
}initStates_t;

/* LCD scheduler processes */
typedef enum
{
	idleProcess,
	clearProcess,
	writeProcess,
	moveProcess
}process_t;

/* LCD private functions declarations */
static STD_STATUS HLCD_u8Write(u8 Copy_u8Value, u8 Copy_u8WriteType);
static STD_STATUS HLCD_u8InitProcess(void);
static STD_STATUS HLCD_u8WriteProcess(void);
static STD_STATUS HLCD_u8ClearProcess(void);
static STD_STATUS HLCD_u8MoveProcess(void);
static STD_STATUS HLCD_u8SetDataPort(u8 Copy_u8Data);
static STD_STATUS HLCD_u8SetHalfDataPort(u8 Copy_u8Data);

/* LCD Task declaraion */
extern Task_t HLCD_task =
{
	.runnable = HLCD_Task,
	.periodicity_us = 20000 /* 20ms */
};

/* LCD configurations structures */
extern const HLCD_cfgStruct_t HLCD_cfgStruct;
extern const HLCD_cfgInitStateStruct_t HLCD_cfgInitStateStruct;

static u8 initComplete;
static process_t currentProcess;

/* LCD cursorn position */
static u8 HLCD_u8CursorLine;
static u8 HLCD_u8CursorPosition;

/* LCD application notifications callback functions */
static callBackFn_t HLCD_writeAppNotify;
static callBackFn_t HLCD_clearAppNotify;
static callBackFn_t HLCD_moveAppNotify;

/* LCD copy buffer variables */
static u8 *HLCD_pu8CopyBuffer;
static u8 HLCD_u32CopyBufferLength;

/****************************************************************************************************************
*                                                                                                               *
*	Description:                                                                                                *
*				This function shall initialize the LCD pins.                                     				*
*				                                                                                                *
*	Input Parameters:                                                                                           *
*				void                                                   											*
*                                                                                                               *
*	Return value:                                                                                               *
*				STD_STATUS: The function's execution status                                                     *
*					Options:                                                                                    *
*					STATUS_NOT_OK >> Function ended with faliure                                                *
*					STATUS_OK >> Function ended with success                                                    *
*                                                                                                               *
****************************************************************************************************************/
extern STD_STATUS HLCD_u8Init(void)
{
	STD_STATUS local_u8ErrorStatus = STATUS_NOT_OK;
	u8 local_u8Index = 0;
	GPIO_t local_gpioConfigStruct;
	local_gpioConfigStruct.speed = OUTPUT_SPEED_10M;
	local_gpioConfigStruct.mode = OUTPUT_PUSH_PULL;
	
	/* Configure the GPIO Pins for LCD as outputs */
	for(local_u8Index = 0; local_u8Index < HLCD_DATA_LINES_NUMBER; local_u8Index++)
	{
		local_gpioConfigStruct.port = (GPIO_registers *)HLCD_cfgStruct.dataPorts[local_u8Index];
		local_gpioConfigStruct.pin = HLCD_cfgStruct.dataPins[local_u8Index];		
		GPIO_u8ConfigurePin(&local_gpioConfigStruct);
	}
	
	local_gpioConfigStruct.port = (GPIO_registers *)HLCD_cfgStruct.RS_Port;
	local_gpioConfigStruct.pin = HLCD_cfgStruct.RS_Pin;
	GPIO_u8ConfigurePin(&local_gpioConfigStruct);
	
	local_gpioConfigStruct.port = (GPIO_registers *)HLCD_cfgStruct.RW_Port;
	local_gpioConfigStruct.pin = HLCD_cfgStruct.RW_Pin;		
	GPIO_u8ConfigurePin(&local_gpioConfigStruct);
	
	local_gpioConfigStruct.port = (GPIO_registers *)HLCD_cfgStruct.E_Port;
	local_gpioConfigStruct.pin = HLCD_cfgStruct.E_Pin;		
	GPIO_u8ConfigurePin(&local_gpioConfigStruct);
	
	/* Resetting the values to 0 */
	HLCD_u8SetDataPort(0x00);
	
	GPIO_WritePinVal((GPIO_registers *)HLCD_cfgStruct.RS_Port, HLCD_cfgStruct.RS_Pin, LOW);
	GPIO_WritePinVal((GPIO_registers *)HLCD_cfgStruct.RW_Port, HLCD_cfgStruct.RW_Pin, LOW);
	GPIO_WritePinVal((GPIO_registers *)HLCD_cfgStruct.E_Port, HLCD_cfgStruct.E_Pin, LOW);
	
	local_u8ErrorStatus = STATUS_OK;
	
	return local_u8ErrorStatus;	
}

static STD_STATUS HLCD_u8Write(u8 Copy_u8Value, u8 Copy_u8WriteType)
{
	STD_STATUS local_u8ErrorStatus = STATUS_NOT_OK;

	switch(Copy_u8WriteType)
	{
		case HLCD_CMD_TYPE:
			GPIO_WritePinVal((GPIO_registers *)HLCD_cfgStruct.RS_Port, HLCD_cfgStruct.RS_Pin, LOW);
			GPIO_WritePinVal((GPIO_registers *)HLCD_cfgStruct.RW_Port, HLCD_cfgStruct.RW_Pin, LOW);
			local_u8ErrorStatus = STATUS_OK;
			break;
			
		case HLCD_DATA_TYPE:
			GPIO_WritePinVal((GPIO_registers *)HLCD_cfgStruct.RS_Port, HLCD_cfgStruct.RS_Pin, HIGH);
			GPIO_WritePinVal((GPIO_registers *)HLCD_cfgStruct.RW_Port, HLCD_cfgStruct.RW_Pin, LOW);
			local_u8ErrorStatus = STATUS_OK;
			break;
		
		
		default:
			local_u8ErrorStatus = STATUS_NOT_OK;
			break;		
	}
	switch(HLCD_cfgStruct.mode)
	{
		case EIGHT_BITS_MODE:
			/* Set Data on Data Port */
			HLCD_u8SetDataPort(Copy_u8Value);
			local_u8ErrorStatus = STATUS_OK;
			break;
			
		case FOUR_BITS_MODE:
			HLCD_u8SetHalfDataPort(Copy_u8Value >> 4);
			
			//pulsate here
			
			HLCD_u8SetHalfDataPort(Copy_u8Value);
			
			//pulsate here
		
			local_u8ErrorStatus = STATUS_OK;
			break;
		
		
		default:
			local_u8ErrorStatus = STATUS_NOT_OK;
			break;		
	}
	return local_u8ErrorStatus;
}

/****************************************************************************************************************
*                                                                                                               *
*	Description:                                                                                                *
*				This function shall write a string on LCD.                                     					*
*				                                                                                                *
*	Input Parameters:                                                                                           *
*				const u8 *Copy_pu8Buffer: Takes a pointer to the string                                			*
*                                                                                                               *
*				u32 Copy_u32Length: Takes length of the string                       							*
*                                                                                                               *
*	Return value:                                                                                               *
*				STD_STATUS: The function's execution status                                                     *
*					Options:                                                                                    *
*					STATUS_NOT_OK >> Function ended with faliure                                                *
*					STATUS_OK >> Function ended with success                                                    *
*					STATUS_NULLPOINTER >> Function ended with nullpointer error                                 *
*					STATUS_BUSY >> Function was busy                                                    		*
*                                                                                                               *
****************************************************************************************************************/
extern STD_STATUS HLCD_u8WriteData(const u8 *Copy_pu8Buffer, u32 Copy_u32Length)
{
	STD_STATUS local_u8ErrorStatus = STATUS_NOT_OK;
	u8 local_u8Index = 0;
	
	if(Copy_pu8Buffer)
	{
		if(Copy_u32Length)
		{
			if(currentProcess == idleProcess)
			{
				currentProcess = writeProcess;
				/*for(local_u8Index = 0; local_u8Index < Copy_u32Length; local_u8Index++)
				{
					HLCD_pu8CopyBuffer[local_u8Index] = Copy_pu8Buffer[local_u8Index];
				}*/
				HLCD_pu8CopyBuffer = HLCD_pu8CopyBuffer;
				HLCD_u32CopyBufferLength = Copy_u32Length;
				local_u8ErrorStatus = STATUS_OK;
			}
			else
			{
				/* Returning that the buffer is busy */
				local_u8ErrorStatus = STATUS_BUSY;
			}
		}
		else
		{
			local_u8ErrorStatus = STATUS_NOT_OK;
		}
	}
	else
	{
		local_u8ErrorStatus = STATUS_NULLPOINTER;
	}
	return local_u8ErrorStatus;
}

/****************************************************************************************************************
*                                                                                                               *
*	Description:                                                                                                *
*				This function shall clear the LCD screen                                     					*
*				                                                                                                *
*	Input Parameters:                                                                                           *
*				void                                															*
*                                                                                                               *
*	Return value:                                                                                               *
*				STD_STATUS: The function's execution status                                                     *
*					Options:                                                                                    *
*					STATUS_NOT_OK >> Function ended with faliure                                                *
*					STATUS_OK >> Function ended with success                                                    *
*					STATUS_BUSY >> Function was busy                                                    		*
*                                                                                                               *
****************************************************************************************************************/
extern STD_STATUS HLCD_u8ClearScreen(void)
{
	STD_STATUS local_u8ErrorStatus = STATUS_NOT_OK;
	
	if(currentProcess == idleProcess)
	{
		currentProcess = clearProcess;	
		local_u8ErrorStatus = STATUS_OK;
	}
	else
	{
		local_u8ErrorStatus = STATUS_BUSY;
	}
	return local_u8ErrorStatus;
}

/****************************************************************************************************************
*                                                                                                               *
*	Description:                                                                                                *
*				This function shall write a string on LCD.                                     					*
*				                                                                                                *
*	Input Parameters:                                                                                           *
*				u8 Copy_u8LineNum: Takes a cursor line                                							*
*                                                                                                               *
*				u8 Copy_u8Position: Takes a crusor position on that line                      					*
*                                                                                                               *
*	Return value:                                                                                               *
*				STD_STATUS: The function's execution status                                                     *
*					Options:                                                                                    *
*					STATUS_NOT_OK >> Function ended with faliure                                                *
*					STATUS_OK >> Function ended with success                                                    *
*					STATUS_BUSY >> Function was busy                                                    		*
*                                                                                                               *
****************************************************************************************************************/
extern STD_STATUS HLCD_u8MoveCursor(u8 Copy_u8LineNum, u8 Copy_u8Position)
{
	STD_STATUS local_u8ErrorStatus = STATUS_NOT_OK;
	if(((Copy_u8LineNum == HLCD_LINE_1_CMD)||(Copy_u8LineNum == HLCD_LINE_2_CMD)) && 
		(Copy_u8Position <= HLCD_MAX_POSITION))
	{
		if(currentProcess == idleProcess)
		{
			currentProcess = moveProcess;
			HLCD_u8CursorLine = Copy_u8LineNum;
			HLCD_u8CursorPosition = Copy_u8Position;
		}
		else
		{
			local_u8ErrorStatus = STATUS_BUSY;
		}
	}
	else
	{
		local_u8ErrorStatus = STATUS_NOT_OK;
	}
	return local_u8ErrorStatus;
}

static STD_STATUS HLCD_u8InitProcess(void)
{
	STD_STATUS local_u8ErrorStatus = STATUS_NOT_OK;
	
	static initStates_t currentState = initS1;
	u8 local_u8EnableValue = 0;
	
	GPIO_GetPinVal((GPIO_registers *)HLCD_cfgStruct.E_Port, HLCD_cfgStruct.E_Pin, &local_u8EnableValue);
	
	switch(currentState)
	{
		case initS1:
			if(local_u8EnableValue)
			{
				/* Clear enable pin */
				GPIO_WritePinVal((GPIO_registers* )HLCD_cfgStruct.E_Port, HLCD_cfgStruct.E_Pin, LOW);
				currentState = initS2;
			}
			else
			{
				/* Send state1 init command */
				HLCD_u8Write(HLCD_cfgInitStateStruct.cfgInitState1, HLCD_CMD_TYPE);
				
				/* Set enable pin */
				GPIO_WritePinVal((GPIO_registers* )HLCD_cfgStruct.E_Port, HLCD_cfgStruct.E_Pin, HIGH);
			}
			break;
			
		case initS2:
			if(local_u8EnableValue)
			{
				/* Clear enable pin */
				GPIO_WritePinVal((GPIO_registers* )HLCD_cfgStruct.E_Port, HLCD_cfgStruct.E_Pin, LOW);
				currentState = initS3;
			}
			else
			{
				/* Send state2 init command */
				HLCD_u8Write(HLCD_cfgInitStateStruct.cfgInitState2, HLCD_CMD_TYPE);
				
				/* Set enable pin */
				GPIO_WritePinVal((GPIO_registers* )HLCD_cfgStruct.E_Port, HLCD_cfgStruct.E_Pin, HIGH);
			}
			break;
		case initS3:
			if(local_u8EnableValue)
			{
				/* Clear enable pin */
				GPIO_WritePinVal((GPIO_registers* )HLCD_cfgStruct.E_Port, HLCD_cfgStruct.E_Pin, LOW);
				currentState = initS4;
			}
			else
			{
				/* Send state3 init command */
				HLCD_u8Write(HLCD_cfgInitStateStruct.cfgInitState3, HLCD_CMD_TYPE);
				
				/* Set enable pin */
				GPIO_WritePinVal((GPIO_registers* )HLCD_cfgStruct.E_Port, HLCD_cfgStruct.E_Pin, HIGH);
			}
			break;
		case initS4:
			if(local_u8EnableValue)
			{
				/* Clear enable pin */
				GPIO_WritePinVal((GPIO_registers* )HLCD_cfgStruct.E_Port, HLCD_cfgStruct.E_Pin, LOW);
				initComplete = 1;
			}
			else
			{
				/* Send state4 init command */
				HLCD_u8Write(HLCD_cfgInitStateStruct.cfgInitState4, HLCD_CMD_TYPE);
				
				/* Set enable pin */
				GPIO_WritePinVal((GPIO_registers* )HLCD_cfgStruct.E_Port, HLCD_cfgStruct.E_Pin, HIGH);
			}
			break;
	}
	local_u8ErrorStatus = STATUS_OK;
	return local_u8ErrorStatus;
}

static STD_STATUS HLCD_u8WriteProcess(void)
{
	STD_STATUS local_u8ErrorStatus = STATUS_NOT_OK;
	static u8 local_u8CurrentChar = 0;
	u8 local_u8EnableValue = 0;
	
	GPIO_u8GetPinValue(HLCD_cfgStruct.E_Port, HLCD_cfgStruct.E_Pin, &local_u8EnableValue);
	
	if(local_u8EnableValue)
	{
		/* Clear enable pin */
		GPIO_u8ResetPin(HLCD_cfgStruct.E_Port, HLCD_cfgStruct.E_Pin);
	}
	else
	{
		/* Check if all chars has been written */
		if(local_u8CurrentChar == HLCD_u32CopyBufferLength)
		{
			currentProcess = idleProcess;
			local_u8CurrentChar = 0;
			if(HLCD_writeAppNotify)
			{
				HLCD_writeAppNotify();	
			}
		}
		else
		{
			HLCD_u8Write(HLCD_pu8CopyBuffer[local_u8CurrentChar], HLCD_DATA_TYPE);
			local_u8CurrentChar++;
			/* Set enable pin */
			GPIO_u8SetPin(HLCD_cfgStruct.E_Port, HLCD_cfgStruct.E_Pin);
		}
	}
	local_u8ErrorStatus = STATUS_OK;
	return local_u8ErrorStatus;
}

static STD_STATUS HLCD_u8ClearProcess(void)
{
	STD_STATUS local_u8ErrorStatus = STATUS_NOT_OK;
	
	u8 local_u8EnableValue = 0;
	
	GPIO_GetPinVal((GPIO_registers *)HLCD_cfgStruct.E_Port, HLCD_cfgStruct.E_Pin, &local_u8EnableValue);
	
	if(local_u8EnableValue)
	{
		/* Clear enable pin */
		GPIO_WritePinVal((GPIO_registers* )HLCD_cfgStruct.E_Port, HLCD_cfgStruct.E_Pin, LOW);
		currentProcess = idleProcess;
		if(HLCD_clearAppNotify)
		{
			HLCD_clearAppNotify();
		}
	}
	else
	{
		HLCD_u8Write(HLCD_CLEAR_CMD, HLCD_CMD_TYPE);
		/* Set enable pin */
		GPIO_WritePinVal((GPIO_registers* )HLCD_cfgStruct.E_Port, HLCD_cfgStruct.E_Pin, HIGH);
	}
	local_u8ErrorStatus = STATUS_OK;
	return local_u8ErrorStatus;
}

static STD_STATUS HLCD_u8MoveProcess(void)
{
	STD_STATUS local_u8ErrorStatus = STATUS_NOT_OK;
	
	u8 local_u8EnableValue = 0;
	
	GPIO_GetPinVal((GPIO_registers *)HLCD_cfgStruct.E_Port, HLCD_cfgStruct.E_Pin, &local_u8EnableValue);
	
	if(local_u8EnableValue)
	{
		/* Clear enable pin */
		GPIO_WritePinVal((GPIO_registers *)HLCD_cfgStruct.E_Port, HLCD_cfgStruct.E_Pin, LOW);
		currentProcess = idleProcess;
		HLCD_u8CursorLine = 0;
		HLCD_u8CursorPosition = 0;
		if(HLCD_moveAppNotify)
		{
			HLCD_moveAppNotify();
		}
	}
	else
	{
		HLCD_u8Write(HLCD_u8CursorLine + HLCD_u8CursorPosition, HLCD_CMD_TYPE);
		/* Set enable pin */
		GPIO_WritePinVal((GPIO_registers *)HLCD_cfgStruct.E_Port, HLCD_cfgStruct.E_Pin, HIGH);
	}
	local_u8ErrorStatus = STATUS_OK;
	return local_u8ErrorStatus;
}

/****************************************************************************************************************
*                                                                                                               *
*	Description:                                                                                                *
*				This function shall call the write complete notification function                               *
*				                                                                                                *
*	Input Parameters:                                                                                           *
*				callBackFn_t writeNotifyCbf: Takes a call back function pointer                                	*
*                                                                                                               *
*	Return value:                                                                                               *
*				STD_STATUS: The function's execution status                                                     *
*					Options:                                                                                    *
*					STATUS_NOT_OK >> Function ended with faliure                                                *
*					STATUS_OK >> Function ended with success                                                    *
*					STATUS_NULLPOINTER >> Function ended with nullpointer error                                 *
*                                                                                                               *
****************************************************************************************************************/
extern STD_STATUS HLCD_u8SetWriteCallbackFn(callBackFn_t writeNotifyCbf)
{
	STD_STATUS local_u8ErrorStatus = STATUS_NOT_OK;
	
	if(writeNotifyCbf)
	{
		HLCD_writeAppNotify = writeNotifyCbf;
		local_u8ErrorStatus = STATUS_OK;
	}
	else
	{
		local_u8ErrorStatus = STATUS_NULLPOINTER;
	}
	
	return local_u8ErrorStatus;
}

/****************************************************************************************************************
*                                                                                                               *
*	Description:                                                                                                *
*				This function shall call the clear complete notification function                               *
*				                                                                                                *
*	Input Parameters:                                                                                           *
*				callBackFn_t clearNotifyCbf: Takes a call back function pointer                                	*
*                                                                                                               *
*	Return value:                                                                                               *
*				STD_STATUS: The function's execution status                                                     *
*					Options:                                                                                    *
*					STATUS_NOT_OK >> Function ended with faliure                                                *
*					STATUS_OK >> Function ended with success                                                    *
*					STATUS_NULLPOINTER >> Function ended with nullpointer error                                 *
*                                                                                                               *
****************************************************************************************************************/
extern STD_STATUS HLCD_u8SetClearCallbackFn(callBackFn_t clearNotifyCbf)
{
	STD_STATUS local_u8ErrorStatus = STATUS_NOT_OK;
	
	if(clearNotifyCbf)
	{
		HLCD_clearAppNotify = clearNotifyCbf;
		local_u8ErrorStatus = STATUS_OK;
	}
	else
	{
		local_u8ErrorStatus = STATUS_NULLPOINTER;
	}
	
	return local_u8ErrorStatus;
}

/****************************************************************************************************************
*                                                                                                               *
*	Description:                                                                                                *
*				This function shall call the move cursor complete notification function                         *
*				                                                                                                *
*	Input Parameters:                                                                                           *
*				callBackFn_t moveNotifyCbf: Takes a call back function pointer                                	*
*                                                                                                               *
*	Return value:                                                                                               *
*				STD_STATUS: The function's execution status                                                     *
*					Options:                                                                                    *
*					STATUS_NOT_OK >> Function ended with faliure                                                *
*					STATUS_OK >> Function ended with success                                                    *
*					STATUS_NULLPOINTER >> Function ended with nullpointer error                                 *
*                                                                                                               *
****************************************************************************************************************/
extern STD_STATUS HLCD_u8SetMoveCallbackFn(callBackFn_t moveNotifyCbf)
{
	STD_STATUS local_u8ErrorStatus = STATUS_NOT_OK;
	
	if(moveNotifyCbf)
	{
		HLCD_moveAppNotify = moveNotifyCbf;
		local_u8ErrorStatus = STATUS_OK;
	}
	else
	{
		local_u8ErrorStatus = STATUS_NULLPOINTER;
	}
	
	return local_u8ErrorStatus;
}

/****************************************************************************************************************
*                                                                                                               *
*	Description:                                                                                                *
*				This function shall execute the LCD Task                              							*
*				                                                                                                *
*	Input Parameters:                                                                                           *
*				void                                															*
*                                                                                                               *
*	Return value:                                                                                               *
*				STD_STATUS: The function's execution status                                                     *
*					Options:                                                                                    *
*					STATUS_NOT_OK >> Function ended with faliure                                                *
*					STATUS_OK >> Function ended with success                                                    *
*					STATUS_NULLPOINTER >> Function ended with nullpointer error                                 *
*                                                                                                               *
****************************************************************************************************************/
extern void HLCD_Task(void)
{
	if(initComplete)
	{
		switch(currentProcess)
		{
			case idleProcess:	
				break;
				
			case writeProcess:	
				HLCD_u8WriteProcess();
				break;
				
			case clearProcess:	
				HLCD_u8ClearProcess();
				break;
				
			case moveProcess:	
				HLCD_u8MoveProcess();
				break;		
				
			default:
				break;
		}
	}
	else
	{
		HLCD_u8InitProcess();	
	}
}

static STD_STATUS HLCD_u8SetDataPort(u8 Copy_u8Data)
{
	STD_STATUS local_u8ErrorStatus = STATUS_NOT_OK;
	u8 local_u8Index;
	
	for(local_u8Index = 0; local_u8Index < HLCD_DATA_LINES_NUMBER; local_u8Index++)
	{
		GPIO_WritePinVal(
							(GPIO_registers *)HLCD_cfgStruct.dataPorts[local_u8Index], 
							HLCD_cfgStruct.dataPins[local_u8Index], 
							(Copy_u8Data >> local_u8Index) & 0x1
						);
	}
	local_u8ErrorStatus = STATUS_OK;
	return local_u8ErrorStatus;
}

static STD_STATUS HLCD_u8SetHalfDataPort(u8 Copy_u8Data)
{	
	STD_STATUS local_u8ErrorStatus = STATUS_NOT_OK;
	u8 local_u8Index;
	
	for(local_u8Index = HLCD_DATA_LINES_NUMBER/2; local_u8Index < HLCD_DATA_LINES_NUMBER; local_u8Index++)
	{
		GPIO_WritePinVal(
							(GPIO_registers *)HLCD_cfgStruct.dataPorts[local_u8Index], 
							HLCD_cfgStruct.dataPins[local_u8Index], 
							(Copy_u8Data >> local_u8Index) & 0x1
						);
	}
	local_u8ErrorStatus = STATUS_OK;
	return local_u8ErrorStatus;
}

//FUTURE ENHANCEMENT:
// COMPLETE THE 4BIT MODE
// ADD FUNCTIONS TO ACCEPT SPECIAL CHARACHTERS >> adding an extra intit stage and new function for special write
