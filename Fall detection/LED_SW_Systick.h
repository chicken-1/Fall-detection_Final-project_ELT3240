#include "MKL46Z4.h"

void initSystick(void);
void SysTick_Handler(void);
void Delay(uint32_t time);
void initLED_GREEN(void);
void initLED_RED(void);
void initSW1(void);
void initSW2(void);
void PORTC_PORTD_IRQHandler(void);
