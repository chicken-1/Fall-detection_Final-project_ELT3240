#include "MKL46Z4.h"
#include "I2C.h"
#include "LED_SW1.h" 

uint32_t volatile msTicks1 = 0;
uint32_t volatile msTicks2 = 0;

#define NORMAL_MODE 0x00
#define STOP_MODE 0x01
#define FALL_DETECTED_MODE 0x02

uint8_t mode = STOP_MODE;

void SysTick_Handler()
{
	if (mode == NORMAL_MODE)
	{
		PTD->PTOR |= 1 << 5;         // tongle led green
		msTicks1++;
		while(msTicks1 < 1000);
		msTicks1 = 0;
	} else if (mode == FALL_DETECTED_MODE)
	{
		PTE->PTOR |= 1 << 29;				//tongle led red
		msTicks2++;
		while(msTicks2 < 500);
		msTicks2 = 0;
	} else if (mode == STOP_MODE)
	{
		PTD->PDOR |= 1 << 5; 	// turn led green off
		PTE->PDOR |= 1 << 29;	// turn led red off
	}
	
}


void PORTC_PORTD_IRQHandler() 
{
	// if SW1 is pressed
	/*if (PORTC->PCR[3] & 1 << 24) 
	{
		mode = ~mode;
		if (mode) {
		// active mode: led _green blinking
		PTD->PTOR |= 1 << 5;         // tongle led             
		Delay(1000);
		} 
		else {
			PTD->PDOR |= 1 << 5; 	//turn LED off
		}
	}
	
	// if SW2 is pressed
	if (PORTC->PCR[12] & 1 << 24)
	{
		fallCheck = 0x0;
		PTE->PDOR |= 1 << 29;		//turn LED red off
	}*/
	// if SW1 is pressed
	if (PORTC->PCR[3] & 1 << 24)
	{
		if (mode == STOP_MODE)
		{
			mode = NORMAL_MODE;
			SysTick->CTRL |= 1;
		} else if (mode == NORMAL_MODE)
		{
			mode = STOP_MODE;
			//SysTick->CTRL &= 0;
		} else if (mode == FALL_DETECTED_MODE)
		{
			mode = STOP_MODE;
			//SysTick->CTRL &=0;
		}
	}
	
	// if SW2 is pressed - normal mode
	if (PORTC->PCR[12] & 1 << 24)
	{
		
	}
	
}

int main(void)
{
	initLED_RED();
	initLED_GREEN();
	initSW1();
	initSW2();
	InitI2C();
	configMMA84512Q();
	
	while(1)
	{
		
	}
}

