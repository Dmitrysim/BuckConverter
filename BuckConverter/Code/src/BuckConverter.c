#include "BuckConverter.h"

uint16_t adcResult = 0;
uint16_t dutyControl = 0;

/*********************************** Used functions *************************************/

void InitHRPWM (void) {

	RCC->CFGR3   |= RCC_CFGR3_HRTIM1SW_PLL;
	RCC->APB2ENR |= RCC_APB2ENR_HRTIM1EN;

	/************************************************
	 *                Setting GPIO
	 ***********************************************/

	RCC->AHBENR  |= RCC_AHBENR_GPIOAEN;

	GPIOA->MODER   &= ~GPIO_MODER_MODER8;
	GPIOA->MODER   |= GPIO_MODER_MODER8_1;																		// Alternative PP
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR8;																	// Very high speed

	GPIOA->MODER   &= ~GPIO_MODER_MODER9;
	GPIOA->MODER   |= GPIO_MODER_MODER9_1;
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR9;

	GPIOA->AFR[1] |= 0xDD;																						// PA8 and PA9 - AF13

	/************************************************
	 *               Setting timer A
	 ***********************************************/

	HRTIM1->sCommonRegs.DLLCR |= HRTIM_DLLCR_CAL | HRTIM_DLLCR_CALEN;
	while ((HRTIM1->sCommonRegs.ISR & HRTIM_ISR_DLLRDY) == RESET);

	HRTIM1->sTimerxRegs[0].PERxR = PeriodTimerA;																// Period for timer A
	HRTIM1->sTimerxRegs[0].CMP1xR = 0;																			// Duty for timer A

	HRTIM1->sTimerxRegs[0].OUTxR |= HRTIM_OUTR_DTEN;															// Deadtime enable
	HRTIM1->sTimerxRegs[0].DTxR  |= HRTIM_DTR_DTPRSC_0 | HRTIM_DTR_DTPRSC_1;									// Tdtg = 6.94 ns
	HRTIM1->sTimerxRegs[0].DTxR  |= HRTIM_DTR_DTR_0 | HRTIM_DTR_DTR_1 | HRTIM_DTR_DTR_2 | HRTIM_DTR_DTR_3;		// Deadtime rising = 15*Ttg = 104 ns
	HRTIM1->sTimerxRegs[0].DTxR  |= HRTIM_DTR_DTF_0 | HRTIM_DTR_DTF_1 | HRTIM_DTR_DTF_2 | HRTIM_DTR_DTF_3;		// Deadtime falling = 15*Ttg = 104 ns
	HRTIM1->sTimerxRegs[0].DTxR  |= HRTIM_DTR_DTFSLK | HRTIM_DTR_DTRSLK;

	HRTIM1->sTimerxRegs[0].SETx1R |= HRTIM_SET1R_PER;															// Event forces the output to active state for TA1
	HRTIM1->sTimerxRegs[0].RSTx1R |= HRTIM_RST1R_CMP1;															// Event forces the output to inactive state for TA1

	/************************************************
	* ADC trigger intialization (with CMP2 event)
	************************************************/

	HRTIM1->sTimerxRegs[0].CMP2xR = PeriodTimerA / 10;															// Samples in middle of ON time
	HRTIM1->sCommonRegs.CR1 |= HRTIM_CR1_ADC1USRC_0;															// ADC trigger 1 update: Timer A
	HRTIM1->sCommonRegs.ADC1R |= HRTIM_ADC1R_AD1TAC2;															// ADC trigger 1 event: Timer A compare 2

	/************************************************
	 *                 HRTIM start
	 ***********************************************/

	HRTIM1->sCommonRegs.OENR |= HRTIM_OENR_TA1OEN | HRTIM_OENR_TA2OEN;											// Enable output PWM for TA1 and TA2

	HRTIM1->sTimerxRegs[0].TIMxCR |= HRTIM_TIMCR_CONT;															// Continuous mode

	HRTIM1->sMasterRegs.MPER = 65000;																			// Period for master timer
	HRTIM1->sMasterRegs.MCR |= HRTIM_MCR_MCEN | HRTIM_MCR_TACEN;												// Enable counter for Master and timer A

}

void SetDutyTimerA (uint16_t duty) {

	HRTIM1->sTimerxRegs[0].CMP1xR = duty;

}

void InitBasicADC (void) {

	RCC->AHBENR |= RCC_AHBENR_ADC12EN;
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

	/************************************************
	 *                 Calibration
	 ***********************************************/

	ADC2->CR &= ~ADC_CR_ADVREGEN;
	ADC2->CR |= ADC_CR_ADVREGEN_0;													// Vref enable
	Delay(10);
	ADC2->CR &= ~ADC_CR_ADCALDIF;

	ADC2->CR |= ADC_CR_ADCAL;														// Start calibration
	while (ADC2->CR & ADC_CR_ADCAL);												// Wait end calibration


	/************************************************
	 *       Select event trigger and channel
	 ***********************************************/

	ADC2->CFGR |= ADC_CFGR_EXTEN_0;													// Enable start conversion external trigger
	ADC2->CFGR |= ADC_CFGR_EXTSEL_0 | ADC_CFGR_EXTSEL_1 | ADC_CFGR_EXTSEL_2;		// Event 7 - HRTIM

	ADC2->SQR1 |= ADC_SQR1_SQ1_0 | ADC_SQR1_SQ1_2;									// Select ADC2 channel IN5
	ADC2->SQR1 &= ~ADC_SQR1_L;														// Length regular ADC channel = 1

	ADC2->IER |= ADC_IER_EOCIE;														// Interrupt enable
	NVIC_EnableIRQ(ADC1_2_IRQn); 													// enable interrupt ADC1 and ADC2

	/************************************************
	 *                 Start ADC
	 ***********************************************/

	ADC2->CR |= ADC_CR_ADEN;														// Enable ADC2
	Delay(10);
	ADC2->CR |= ADC_CR_ADSTART;

}

void ADC1_2_IRQHandler (void) {

	ADC2->ISR |= ADC_ISR_EOC;

	adcResult = ADC2->DR;

	if (adcResult > 2000) {

		dutyControl = dutyControl - 10;
	}
	else
	{
		dutyControl = dutyControl + 10;
	}

	SetDutyTimerA(dutyControl);
}





































