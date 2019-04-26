#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "arm_math.h"                   // ARM::CMSIS:DSP

/************************************* Defines and prototypes ************************************/

#define SIG_LENGTH				320
#define IMP_RSP_LENGTH		29

uint32_t freq;

extern void SystemClock_Config (void);
extern float32_t inputSignal_f32_1kHz_15kHz[SIG_LENGTH];

void plot_input_signal(void);
void plot_impulse_response (void);
void plot_output_signal (void);
void convolution ( float32_t * sig_src_arr, float32_t * sig_dest_arr, float32_t * imp_response_arr, uint32_t 	 sig_src_length,uint32_t 	 imp_response_length);

float32_t inputSample, imp_resSample, outputSample;

float32_t output_signal_arr [IMP_RSP_LENGTH + SIG_LENGTH];


const float32_t impulse_response[IMP_RSP_LENGTH] = {
  -0.0018225230f, -0.0015879294f, +0.0000000000f, +0.0036977508f, +0.0080754303f, +0.0085302217f, -0.0000000000f, -0.0173976984f,
  -0.0341458607f, -0.0333591565f, +0.0000000000f, +0.0676308395f, +0.1522061835f, +0.2229246956f, +0.2504960933f, +0.2229246956f,
  +0.1522061835f, +0.0676308395f, +0.0000000000f, -0.0333591565f, -0.0341458607f, -0.0173976984f, -0.0000000000f, +0.0085302217f,
  +0.0080754303f, +0.0036977508f, +0.0000000000f, -0.0015879294f, -0.0018225230f
};

/*************************************** use functions *******************************************/

int main(void) {
	
	HAL_Init();
	SystemClock_Config();
	freq = HAL_RCC_GetHCLKFreq();
	
	//plot_input_signal();
	//plot_impulse_response ();
	convolution ((float32_t *) &inputSignal_f32_1kHz_15kHz[0],
							 (float32_t *) &output_signal_arr[0],
							 (float32_t *) &impulse_response[0],
							 (uint32_t) 	 SIG_LENGTH,
							 (uint32_t) 	 IMP_RSP_LENGTH);
	
	plot_output_signal();
	
	while (1) {
		
		
		
	}
	
}


void plot_input_signal(void) {
	
	int i, j;
	for (i = 0; i < SIG_LENGTH; i++)
	{
		inputSample = inputSignal_f32_1kHz_15kHz[i];
		for (j = 0; j < 3000; j++) {}
	}
	
}

void plot_impulse_response (void) {
	
	int i, j;
	for (i = 0; i < IMP_RSP_LENGTH; i++)
	{
		imp_resSample = impulse_response[i];
		for (j = 0; j < 3000; j++) {}
	}
	
}

void plot_output_signal (void) {
	
	int i, j;
		for (i = 0; i < (IMP_RSP_LENGTH + SIG_LENGTH); i++)
	{
		outputSample = output_signal_arr[i];
		for (j = 0; j < 3000; j++);
	}
	
}

void convolution ( float32_t * sig_src_arr,
									 float32_t * sig_dest_arr,
									 float32_t * imp_response_arr,
									 uint32_t 	 sig_src_length,
									 uint32_t 	 imp_response_length
									) 
{

	uint8_t i, j;
	  for (i = 0; i < (sig_src_length + imp_response_length); i++)
		sig_dest_arr[i] = 0;
			for (i = 0; i < sig_src_length; i++)
			{
				for (j = 0; j < imp_response_length; j++)
				{
					sig_dest_arr[i + j] = sig_dest_arr[i + j] + sig_src_arr[i] * imp_response_arr[i];
				}
			}
		
										
}

float32_t signal_mean(float32_t *sig_src_arr, uint32_t sig_length) {
	
	float32_t _mean = 0.0;
	uint32_t i;
	for (i = 0; i < sig_length; i++)
	{
		_mean = _mean + sig_src_arr[i];
	}
	_mean = _mean/(float32_t)sig_length;
	
	return _mean;
	
}

float32_t signal_variance(float32_t *sig_src_arr, float32_t sig, uint32_t sig_length) {
	
	float32_t _variance = 0.0;
	uint32_t i;
	for (i=0; i < sig_length; i++)
	{
		
		_variance = _variance + powf((sig_src_arr[i] - sig), 2);
		
	}
	
	_variance = _variance /(sig_length - 1);
	return _variance;
	
}

float32_t signal_std(float32_t sign_variance) {
	
	float32_t std = sqrtf(sign_variance);
	
	return std;
	
}

void SysTick_Handler (void) {
	
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
	
}


