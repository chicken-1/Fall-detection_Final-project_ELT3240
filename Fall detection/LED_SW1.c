#include "MKL46Z4.h"
#include "LED_SW1.h"

void initLED_RED()
{
	SIM->SCGC5 |= 1 << 13;		// enable clock to PORTE
	PORTE->PCR[29] |= 1 << 8;	// set PTE29 to GPIO
	PTE->PDDR |= 1 << 29;			// set PTE29 as an output 
}

void initLED_GREEN()
{
	SIM->SCGC5 |= 1 << 12;		//enables clock to port D
	PORTD->PCR[5] |= 1 << 8;	//set PTD5 to GPIO
	PTD->PDDR |= 1 << 5;			//set PTD5 as an output
}

void initSW1()
{
	SIM->SCGC5 |= 1 << 11;												//enables clock to port C
 	PORTC->PCR[3] |= (1 << 8 | 1 << 1 | 1);				//set PTC3 to GPIO, enable pull-up & pull-select 
	PTC->PDDR &= 0 << 3; 													//set PTC3 as an input
	PORTC->PCR[3] |= 1010 << 16; 									//select falling edge interrupts for PORTC[3] (trigger interrupt when press SW1)
	NVIC_ClearPendingIRQ(31);											//clear NVIC any pending interrupts on PORTC_D
	NVIC_EnableIRQ(31);														//enable NVIC interrupts source for PORTC_D
}

void initSW2()
{
	SIM->SCGC5 |= 1 << 11;												//enables clock to port C
 	PORTC->PCR[12] |= (1 << 8 | 1 << 1 | 1);			//set PTC12 to GPIO, enable pull-up & pull-select 
	PTC->PDDR &= 0 << 12; 												//set PTC12 as an input
	PORTC->PCR[12] |= 1010 << 16; 								//select falling edge interrupts for PORTC[12] (trigger interrupt when press SW2)
	NVIC_ClearPendingIRQ(31);											//clear NVICany pending interrupts on PORTC_D
	NVIC_EnableIRQ(31);														// enable NVIC interrupts source for PORTC_D

}

void initSystick()
{
	SysTick->LOAD = SystemCoreClock / 1000 - 1;			 	//configured the SysTick to count in 1ms
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |SysTick_CTRL_TICKINT_Msk 
									|SysTick_CTRL_ENABLE_Msk;		 	//Select Core Clock & Enable SysTick & Enable Interrupt
}

