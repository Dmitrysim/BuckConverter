#include "main.h"

extern uint16_t adcResult;
char buffer[20];

int main (void) {

	InitBasicPeripherals();
//	EnableOutputMCO();

//	UseFaultLED (Fault1, Enable);
//	UseFaultLED (Fault2, Enable);
//	UseFaultLED (Fault3, Enable);
//	UseFaultLED (Fault4, Enable);
//	UseFaultLED (Fault5, Enable);

	UseSignalBuzzer(Disable);

	InitHRPWM();
	InitBasicADC();


	while(1)
	{

		Delay(1000);
		UseStatusLED(Enable);
		Delay(1000);
		UseStatusLED(Disable);

		sprintf(buffer, "%d\r\n", adcResult);
		SendStringUSART1(buffer);

	}
}



