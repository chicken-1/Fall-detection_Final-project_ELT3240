/*
 * seg_lcd.h
 *
 */

#ifndef SOURCES_SEG_LCD_H_
#define SOURCES_SEG_LCD_H_

#include "MKL46Z4.h"
#include "stdbool.h"

/* Front plane waveform bits for each segments (for FRDM-KL46 board. Refer LCD datasheet) */
#define SEG_D		0x11
#define SEG_E		0x22
#define SEG_G		0x44
#define SEG_F		0x88
#define SEG_DP		0x11
#define SEG_COL		0x11
#define SEG_C		0x22
#define SEG_B		0x44
#define SEG_A		0x88
#define SEG_NONE	0x00

/* Funtions */
void slcd_init(void);
void slcd_set_digit(uint32_t digit, uint32_t val);
void slcd_clear_digit(uint32_t digit);
void slcd_clear_all(void);
void slcd_print_fall_msg(void);
void slcd_print_on_msg(void);
void slcd_display_decimal(uint32_t num);


#endif /* SOURCES_SEG_LCD_H_ */