#include "MKL46Z4.h"
#include "I2C.h"
#include "LED_SW_Systick.h" 
#include "seg_lcd.h"

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
		//
		msTicks1++;
		if (msTicks1 == 1000)
		{
			PTD->PTOR |= 1 << 5;         // tongle led green
			slcd_print_on_msg();
			msTicks1 = 0;
		}
		PTE->PDOR |= 1 << 29;					// turn off led red
	} else if (mode == FALL_DETECTED_MODE)
	{
		msTicks2++;
		if (msTicks2 == 500)
		{
			PTE->PTOR |= 1 << 29;				//tongle led red
			slcd_print_fall_msg();
			msTicks2 = 0;
		}
		msTicks1++;
		if (msTicks1 == 1000)
		{
			PTD->PTOR |= 1 << 5;         // tongle led green
			msTicks1 = 0;
		}
	} else if (mode == STOP_MODE)
	{
		PTD->PDOR |= 1 << 5; 	// turn led green off
		PTE->PDOR |= 1 << 29;	// turn led red off
		slcd_clear_all();
	}
	
}


void PORTC_PORTD_IRQHandler() 
{
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
		} else if (mode == FALL_DETECTED_MODE)
		{
			mode = STOP_MODE;
		}
		PTD->PCOR |= 1 << 5;  
		PORTC->PCR[3] |=   1 << 24; 	// clear interrupt
	}
	
	// if SW2 is pressed - normal mode
	if (PORTC->PCR[12] & 1 << 24)
	{
		mode = NORMAL_MODE;
		PTD->PCOR |= 1 << 5;  
		PORTC->PCR[12] |=   1 << 24; 	// clear interrupt
	}
	
	// if detect fall, interrupt in PTD1
	if (PORTD->PCR[1] & 1 << 24)
	{
		if (mode == NORMAL_MODE)
		{
			mode = FALL_DETECTED_MODE;
		}
		PORTD->PCR[1] |=   1 << 24; 	// clear interrupt
		uint8_t dt = receive(0x16);		// read 0x16 -> clear interrupt on mma
	}	
	
}

int main(void)
{
	initLED_RED();
	initLED_GREEN();
	initSW1();
	initSystick();
	initSW2();
	initI2C();
	initINT2();
	configMMA84512Q();
	slcd_init();
	while(1)
	{
		//data = receive(0x0D);
	}
}

