#pragma once

#include "stm32f3xx.h"

/**************************************** Desfines *********************************/

#define PeriodTimerA 	((uint16_t)45000)		// f = 102 kHz

/************************************** Used functions *****************************/

void InitHRPWM (void);
void SetDutyTimerA (uint16_t duty);

void InitBasicADC (void);

extern void Delay (uint32_t setDelay);

