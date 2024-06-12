/*
 * seg_lcd.c
 *
 */

#include "seg_lcd.h"
#include "MKL46Z4.h"


/* LCD front plane pin numbers for each LCD digits on FRDM-KL46 board */
static uint8_t lcd_fp_pins[4][2] = { { 37, 17 }, /* Digit 1 pins in order (first pin drives D,E,F,G and second pin DP,A,B,C segments) */
										{ 7, 8 }, /* Digit 2 pins in order*/
										{ 53, 38 }, /* DIgit 3 pins in order */
										{ 10, 11 } /* DIgit 3 pins in order */
};

/* This function initialize the Segment LCD on FRDM-KL46Z board */
void slcd_init(void)
{
	// Enabling MCGIRCLK clock source to give the LCD 32.768kHz clock for its operation.
	MCG->C1  |= MCG_C1_IRCLKEN_MASK | MCG_C1_IREFSTEN_MASK;
	/* Enable SLCD and Port clocks */
	SIM->SCGC5 |=
			SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK | SIM_SCGC5_SLCD_MASK;

	LCD->GCR |= LCD_GCR_PADSAFE_MASK; /* disable LCD pins while configuring */
	LCD->GCR &= ~LCD_GCR_LCDEN_MASK; /* disable driving LCD  */

 	/* Pin muxing for slcd on FRDM-KL46 board */
	// From Reference manual, set pins to MUX 0 for normal LCD display operation, only use MUX 7 if using LCD fault detection
 	PORTB->PCR[7]  = PORT_PCR_MUX(0u);     //Set PTB7 to LCD_P7
 	PORTB->PCR[8]  = PORT_PCR_MUX(0u);     //Set PTB8 to LCD_P8
 	PORTB->PCR[10] = PORT_PCR_MUX(0u);     //Set PTB10 to LCD_P10
 	PORTB->PCR[11] = PORT_PCR_MUX(0u);     //Set PTB11 to LCD_P11
 	PORTB->PCR[21] = PORT_PCR_MUX(0u);     //Set PTB21 to LCD_P17
 	PORTB->PCR[22] = PORT_PCR_MUX(0u);     //Set PTB22 to LCD_P18
 	PORTB->PCR[23] = PORT_PCR_MUX(0u);     //Set PTB23 to LCD_P19
 	PORTC->PCR[17] = PORT_PCR_MUX(0u);     //Set PTC17 to LCD_P37
 	PORTC->PCR[18] = PORT_PCR_MUX(0u);     //Set PTC18 to LCD_P38
 	PORTD->PCR[0]  = PORT_PCR_MUX(0u);     //Set PTD0 to LCD_P40
 	PORTE->PCR[4]  = PORT_PCR_MUX(0u);     //Set PTE4 to LCD_P52
 	PORTE->PCR[5]  = PORT_PCR_MUX(0u);     //Set PTE5 to LCD_P53

	/* LCD control register */
	LCD->GCR = LCD_GCR_CPSEL_MASK | /* use capacitor charge pump */
				LCD_GCR_SOURCE_MASK | /* Use alternate clock source */
				LCD_GCR_ALTSOURCE(0) | /* Use alternate clock option 0 */
				LCD_GCR_ALTDIV(0) | /* alternate clock divider = 1 */
				LCD_GCR_DUTY(3) | /* 1/4 duty cycle */
				LCD_GCR_LADJ(3) | /* slowest clock source for capacitor charge pump */
				//LCD_GCR_FFR_MASK | /* use fast frame rate */
				LCD_GCR_LCDDOZE(0) | /* allow running in stop mode */
				LCD_GCR_LCLK(4) | /* for getting frame freq = 32, assuming LCD clock = 32kHz */
				LCD_GCR_RVEN_MASK | /* Enable regulated voltage and trim */
				LCD_GCR_RVTRIM(8) |
				LCD_GCR_VSUPPLY_MASK;

	LCD->AR = 	0; /* Disable blink  */

	/* Enable LCD pins for FRDM-KL46 board */
	LCD->PEN[0] = 	(1u << 7) | /* LCD_P7 = FP Digit 2 (D,E,G,F) */
					(1u << 8) | /* LCD_P8  = FP Digit 2 (DP,C,B,A)*/
					(1u << 10) | /* LCD_P10 = FP Digit 4 (D,E,G,F)*/
					(1u << 11) | /* LCD_P11 = FP Digit 4 (COL,C,B,A) */
					(1u << 17) | /* LCD_P17 = FP Digit 1 (DP,C,B,A) */
					(1u << 18) | /* LCD_P18 = BP 4 */
					(1u << 19); /* LCD_P19 = BP 3 */

	LCD->PEN[1] = (1u << 5) | /* LCD_P37 = FP Digit 1 (D,E,G,F) */
					(1u << 6) | /* LCD_P38  = FP Digist 3 (DP,C,B,A) */
					(1u << 8) | /* LCD_P40 = BP 1 */
					(1u << 20) | /* LCD_P52 = BP 2 */
					(1u << 21); /* LCD_P53 = FP Digit 3 (D,E,G,F)*/

	/* Configure backplane and frontplane pins for FRDM-KL46 board */
	LCD->BPEN[0] = (1u << 18) | (1u << 19); /* LCD_P18 and LCD_P19 are back plane pins */
	LCD->BPEN[1] = (1u << 8) | (1u << 20); /* LCD_P40 and LCD_P52 are backplane pins */

	/* Initalize waveform registers to show a blank display */
	LCD->WF[0] = 0; /* Pins 3 to 0 not used */
	LCD->WF[1] = LCD_WF_WF7(0x0); /* LCD_P7 off */
	LCD->WF[2] = LCD_WF_WF8(0x0) | /* LCD_P8 off */
	LCD_WF_WF10(0x0) | /* LCD_P10 off */
	LCD_WF_WF11(0x0); /* LCD_P11 off */
	LCD->WF[3] = 0; /* Pins 12 to 15 not used */
	LCD->WF[4] = LCD_WF_WF17(0x0) | /* LCD_P17 off */
	LCD_WF_WF18(0x88) | /* Backplane pin LCD_P18(BP4) active during H and D phases */
	LCD_WF_WF19(0x44); /* Backplane pin LCD_P19(BP3) active during G and C phases */
	LCD->WF[5] = 0; /* Pins 20 to 23 not used */
	LCD->WF[6] = 0; /* Pins 24 to 27 not used */
	LCD->WF[7] = 0; /* Pins 28 to 31 not used */
	LCD->WF[8] = 0; /* Pins 32 to 35 not used */
	LCD->WF[9] = LCD_WF_WF37(0x0) | /* LCD_P37 off */
	LCD_WF_WF38(0x0); /* LCD_P38 off */
	LCD->WF[10] = LCD_WF_WF40(0x11); /* BP pin LCD_P40 drives A and E phases */
	LCD->WF[11] = 0; /* pins 44 to 47 not used */
	LCD->WF[12] = 0; /* pins 48 to 51 not used */
	LCD->WF[13] = LCD_WF_WF52(0x22) | /* BP pin PCD_P52 drives B and F phases */
	LCD_WF_WF53(0); /* LCD_P53 off */
	LCD->WF[14] = 0; /* Pins 56 to 59 not used */
	LCD->WF[15] = 0; /* Pins 60 to 63 not used */

	/* Disable Pad safe bit and enable LCD driver */
	LCD->GCR &= ~LCD_GCR_PADSAFE_MASK;
	LCD->GCR |= LCD_GCR_LCDEN_MASK;

}

/* Functions to drive a digit to a particular value
 *
 * 	Digit: 0 - 3 position of the digit (0 - leftmost in the display, DIG 1)
 * 	Value: 0x0 to 0xF
 */
void slcd_set_digit(uint32_t digit, uint32_t val)
{
	if ((digit > 3) || (val > 0xF))
		return;

	switch (val) {
		case 0x0:
			LCD->WF8B[lcd_fp_pins[digit][0]] = SEG_D | SEG_E | SEG_F;
			LCD->WF8B[lcd_fp_pins[digit][1]] = SEG_A | SEG_B | SEG_C;
			break;

		case 0x1:
			LCD->WF8B[lcd_fp_pins[digit][0]] = SEG_NONE;
			LCD->WF8B[lcd_fp_pins[digit][1]] = SEG_B | SEG_C;
			break;

		case 0x2:
			LCD->WF8B[lcd_fp_pins[digit][0]] = SEG_D | SEG_G | SEG_E;
			LCD->WF8B[lcd_fp_pins[digit][1]] = SEG_A | SEG_B;
			break;

		case 0x3:
			LCD->WF8B[lcd_fp_pins[digit][0]] = SEG_D | SEG_G;
			LCD->WF8B[lcd_fp_pins[digit][1]] = SEG_A | SEG_B | SEG_C;
			break;

		case 0x4:
			LCD->WF8B[lcd_fp_pins[digit][0]] = SEG_F | SEG_G;
			LCD->WF8B[lcd_fp_pins[digit][1]] = SEG_B | SEG_C;
			break;

		case 0x5:
			LCD->WF8B[lcd_fp_pins[digit][0]] = SEG_F | SEG_G | SEG_D;
			LCD->WF8B[lcd_fp_pins[digit][1]] = SEG_A | SEG_C;
			break;

		case 0x6:
			LCD->WF8B[lcd_fp_pins[digit][0]] = SEG_D | SEG_E | SEG_F | SEG_G;
			LCD->WF8B[lcd_fp_pins[digit][1]] = SEG_A | SEG_C;
			break;

		case 0x7:
			LCD->WF8B[lcd_fp_pins[digit][0]] = SEG_NONE;
			LCD->WF8B[lcd_fp_pins[digit][1]] = SEG_A | SEG_B | SEG_C;
			break;

		case 0x8:
			LCD->WF8B[lcd_fp_pins[digit][0]] = SEG_D | SEG_E | SEG_F | SEG_G;
			LCD->WF8B[lcd_fp_pins[digit][1]] = SEG_A | SEG_B | SEG_C;
			break;

		case 0x9:
			LCD->WF8B[lcd_fp_pins[digit][0]] = SEG_D | SEG_F | SEG_G;
			LCD->WF8B[lcd_fp_pins[digit][1]] = SEG_A | SEG_B | SEG_C;
			break;

		case 0xA:
			LCD->WF8B[lcd_fp_pins[digit][0]] = SEG_E | SEG_F | SEG_G;
			LCD->WF8B[lcd_fp_pins[digit][1]] = SEG_A | SEG_B | SEG_C;
			break;

		case 0xB:
			LCD->WF8B[lcd_fp_pins[digit][0]] = SEG_D | SEG_E | SEG_F | SEG_G;
			LCD->WF8B[lcd_fp_pins[digit][1]] = SEG_C;
			break;

		case 0xC:
			LCD->WF8B[lcd_fp_pins[digit][0]] = SEG_D | SEG_E | SEG_F;
			LCD->WF8B[lcd_fp_pins[digit][1]] = SEG_A;
			break;

		case 0xD:
			LCD->WF8B[lcd_fp_pins[digit][0]] = SEG_D | SEG_E | SEG_G;
			LCD->WF8B[lcd_fp_pins[digit][1]] = SEG_B | SEG_C;
			break;

		case 0xE:
			LCD->WF8B[lcd_fp_pins[digit][0]] = SEG_D | SEG_E | SEG_F | SEG_G;
			LCD->WF8B[lcd_fp_pins[digit][1]] = SEG_A;
			break;

		case 0xF:
			LCD->WF8B[lcd_fp_pins[digit][0]] = SEG_E | SEG_F | SEG_G;
			LCD->WF8B[lcd_fp_pins[digit][1]] = SEG_A;
			break;

		default:
			break;

	}
}

/* Function to clear a digit at given position
 * 	digit : clear this digit
 * Note: does not clear decimal point or colon
 */
void slcd_clear_digit(uint32_t digit)
{
	if(digit < 4) {
		LCD->WF8B[lcd_fp_pins[digit][0]] = 0x0;
		LCD->WF8B[lcd_fp_pins[digit][1]] &= ~(SEG_A|SEG_B|SEG_C);
	}
}

/* Function to clear all front plane pins */
void slcd_clear_all(void)
{
	for(int i = 0; i < 4; i++) 
	{
		LCD->WF8B[lcd_fp_pins[i][0]] = 0x0;
		LCD->WF8B[lcd_fp_pins[i][1]] = 0x0;
	}
}

/* Function to display "FALL" message */
void slcd_print_fall_msg(void)
{
	slcd_clear_all();
	// F
	LCD->WF8B[lcd_fp_pins[0][0]] = SEG_E | SEG_F | SEG_G;
	LCD->WF8B[lcd_fp_pins[0][1]] = SEG_A;
	
	// A
	LCD->WF8B[lcd_fp_pins[1][0]] = SEG_E | SEG_F | SEG_G;
	LCD->WF8B[lcd_fp_pins[1][1]] = SEG_A | SEG_B | SEG_C;
	
	// L
	LCD->WF8B[lcd_fp_pins[2][0]] = SEG_D | SEG_E | SEG_F;
	LCD->WF8B[lcd_fp_pins[2][1]] = 0x0;
	
	// L
	LCD->WF8B[lcd_fp_pins[3][0]] = SEG_D | SEG_E | SEG_F;
	LCD->WF8B[lcd_fp_pins[3][1]] = 0x0;
}

/* Function to display "ON" message */
void slcd_print_on_msg(void)
{
	slcd_clear_all();
	// O
	LCD->WF8B[lcd_fp_pins[2][0]] = SEG_D | SEG_E | SEG_F;
	LCD->WF8B[lcd_fp_pins[2][1]] = SEG_A | SEG_B | SEG_C;
	
	// N
	LCD->WF8B[lcd_fp_pins[3][0]] = SEG_E | SEG_F;
	LCD->WF8B[lcd_fp_pins[3][1]] = SEG_A | SEG_B | SEG_C;
}

/* Funtion to display decimal number (no leading zeros)
 * 	num : number less than 10000
 */
void slcd_display_decimal(uint32_t num)
{
	uint32_t digit;
	bool lead_zero = false;

	digit = num/1000;
	if(!digit) {
		lead_zero = true;
		slcd_clear_digit(0);
	}
	else {
		slcd_set_digit(0, digit);
	}

	num -= digit*1000;
	digit = num/100;
	if(!digit && (true == lead_zero)) {
		slcd_clear_digit(1);
	}
	else {
		slcd_set_digit(1, digit);
		lead_zero = false;
	}

	num -= digit*100;
	digit = num/10;
	if(!digit && (true == lead_zero)) {
		slcd_clear_digit(2);
	}
	else {
		slcd_set_digit(2, digit);
		lead_zero = false;
	}

	num -= digit*10;
	digit = num;
	slcd_set_digit(3, digit);
}