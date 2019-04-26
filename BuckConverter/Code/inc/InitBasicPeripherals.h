#pragma once

#include "stm32f3xx.h"

/**************************************** Defines ********************************/

typedef enum {
	Disable = 0,
	Enable = 1
} status;

typedef enum {
	Fault1 = 1,
	Fault2 = 2,
	Fault3 = 3,
	Fault4 = 4,
	Fault5 = 5
} numberFault;

/************************************** Used functions ***************************/

void InitBasicPeripherals (void);

void InitFaultLED (void);
void UseFaultLED (numberFault number, status statusLED);
void InitStatusLED (void);
void UseStatusLED (status statusLED);

void StartInitClock (void);
void EnableOutputMCO (void);

void InitSignalBuzzer (void);
void UseSignalBuzzer (status statusBuzzer);

void InitSysTick (void);
void Delay (uint32_t setDelay);

void InitBasicUSART1 (void);
void SendDataUSART1 (uint8_t data);
void SendCharUSART1 (char chr);
void SendStringUSART1 (char* str);
