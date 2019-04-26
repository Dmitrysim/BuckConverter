#include "InitBasicPeripherals.h"

/*********************************** Used variable ********************************/

volatile uint32_t delayCounter = 0;

/*********************************** Used functions *******************************/

void InitBasicPeripherals (void) {

	InitFaultLED();
	InitStatusLED();
	StartInitClock();
	InitSignalBuzzer();
	InitSysTick();
	InitBasicUSART1();

}

void InitFaultLED (void) {

	RCC->AHBENR  |= RCC_AHBENR_GPIOCEN;

	GPIOC->MODER &= ~GPIO_MODER_MODER0;
	GPIOC->MODER |= GPIO_MODER_MODER0_0;		// Output PP

	GPIOC->MODER &= ~GPIO_MODER_MODER1;
	GPIOC->MODER |= GPIO_MODER_MODER1_0;		// Output PP

	GPIOC->MODER &= ~GPIO_MODER_MODER2;
	GPIOC->MODER |= GPIO_MODER_MODER2_0;		// Output PP

	GPIOC->MODER &= ~GPIO_MODER_MODER3;
	GPIOC->MODER |= GPIO_MODER_MODER3_0;		// Output PP

	GPIOC->MODER &= ~GPIO_MODER_MODER13;
	GPIOC->MODER |= GPIO_MODER_MODER13_0;		// Output PP

}

void UseFaultLED (numberFault number, status statusLED) {

	if (number == 1) {
		if (statusLED == 0) {GPIOC->BSRR |= GPIO_BSRR_BR_13;}
		if (statusLED == 1) {GPIOC->BSRR |= GPIO_BSRR_BS_13;}
	}

	if (number == 2) {
		if (statusLED == 0) {GPIOC->BSRR |= GPIO_BSRR_BR_0;}
		if (statusLED == 1) {GPIOC->BSRR |= GPIO_BSRR_BS_0;}
	}

	if (number == 3) {
		if (statusLED == 0) {GPIOC->BSRR |= GPIO_BSRR_BR_1;}
		if (statusLED == 1) {GPIOC->BSRR |= GPIO_BSRR_BS_1;}
	}

	if (number == 4) {
		if (statusLED == 0) {GPIOC->BSRR |= GPIO_BSRR_BR_2;}
		if (statusLED == 1) {GPIOC->BSRR |= GPIO_BSRR_BS_2;}
	}

	if (number == 5) {
		if (statusLED == 0) {GPIOC->BSRR |= GPIO_BSRR_BR_3;}
		if (statusLED == 1) {GPIOC->BSRR |= GPIO_BSRR_BS_3;}
	}

}

void InitStatusLED (void) {

	RCC->AHBENR  |= RCC_AHBENR_GPIOBEN;

	GPIOB->MODER &= ~GPIO_MODER_MODER5;
	GPIOB->MODER |= GPIO_MODER_MODER5_0;		// Output PP

}

void UseStatusLED (status statusLED) {

	if (statusLED == 0) {GPIOB->BSRR |= GPIO_BSRR_BR_5;}
	if (statusLED == 1) {GPIOB->BSRR |= GPIO_BSRR_BS_5;}

}

void StartInitClock (void) {

	RCC->CR |= RCC_CR_HSEON; 																// Enable HSE
	while (!(RCC->CR & RCC_CR_HSERDY));

	FLASH->ACR |= FLASH_ACR_LATENCY_1;

	RCC->CFGR  |= RCC_CFGR_PLLMUL9;															// PLL mult x9
	RCC->CFGR  |= RCC_CFGR_PLLSRC;															// Source HSE
	RCC->CFGR2 |= RCC_CFGR2_ADCPRE12_DIV10;													// ADC source AHB/10

	RCC->CR |= RCC_CR_PLLON;
	while((RCC->CR & RCC_CR_PLLRDY) == 0){}

	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_PLL; 															// Select source SYSCLK = PLL
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_1) {} 									// Wait till PLL is used

}

void EnableOutputMCO (void) {

	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;							// Enable clock port A

	GPIOA->MODER   &= ~GPIO_MODER_MODER8;
	GPIOA->MODER   |= GPIO_MODER_MODER8_1;						// Alternative PP
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR8;					// Very high speed

	RCC->CFGR |= RCC_CFGR_MCO_PLL;								// Source PLL
	RCC->CFGR |= RCC_CFGR_PLLNODIV;								// PLL div 1 for MCO

//	RCC->CFGR |= RCC_CFGR_MCO_SYSCLK;							// Source SYSCLK

}

void InitSignalBuzzer (void) {

	RCC->AHBENR  |= RCC_AHBENR_GPIOCEN;

	GPIOC->MODER &= ~GPIO_MODER_MODER10;
	GPIOC->MODER |= GPIO_MODER_MODER10_0;		// Output PP


	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;			// enable clock for basic TIM6

	TIM3->PSC = 18-1;							// div, frequency 4 kHz
	TIM3->ARR = 1000; 							// count to 1000
	TIM3->DIER |= TIM_DIER_UIE; 				// enable interrupt for timer
	TIM3->CR1 |= TIM_CR1_CEN; 					// start count
	NVIC_EnableIRQ(TIM3_IRQn); 					// enable interrupt TIM3_IRQHandler

}

void UseSignalBuzzer (status statusBuzzer) {

	if (statusBuzzer == 0) {TIM3->CR1 &= ~TIM_CR1_CEN;}
	if (statusBuzzer == 1) {TIM3->CR1 |= TIM_CR1_CEN;}

}


void TIM3_IRQHandler (void) {

	TIM3->SR &= ~TIM_SR_UIF;

	GPIOC->ODR ^= GPIO_ODR_10;

}

void InitSysTick (void) {

	SysTick->LOAD = 72000000UL/1000 - 1; 		// 1 ms
	SysTick->VAL = 72000000UL/1000 - 1; 		// Set value

	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk  | SysTick_CTRL_ENABLE_Msk;

}

void SysTick_Handler (void) {

	delayCounter--;

}

void Delay (uint32_t setDelay) {

	delayCounter = setDelay;
	while (delayCounter != 0);

}

void InitBasicUSART1 (void) {

	RCC->AHBENR  |= RCC_AHBENR_GPIOBEN;
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

	/************************************
	 *    UART1: TX - PB6, RX - PB7
	 ***********************************/

	GPIOB->MODER &= ~GPIO_MODER_MODER6;
	GPIOB->MODER |= GPIO_MODER_MODER6_1;		// Output AF PP

	GPIOB->AFR[0] |= (7 << 28) | (7 << 24);		// Alternative function: TX->PB6, RX->PB7

	/************************************
	 *    		Setting UART1
	 ***********************************/

	USART1->BRR = 0x1D4C;
	USART1->CR1 |= USART_CR1_UE | USART_CR1_TE;

}

void SendDataUSART1 (uint8_t data) {

	while (!(USART1->ISR & USART_ISR_TC));
	USART1->TDR = data;

}

void SendCharUSART1 (char chr) {

	while (!(USART1->ISR & USART_ISR_TC));
	USART1->TDR = chr;

}

void SendStringUSART1 (char* str) {

	uint8_t i = 0;

	while(str[i])
	SendCharUSART1 (str[i++]);

}
























