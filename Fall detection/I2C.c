#include "MKL46Z4.h"
#include "I2C.h"

void initI2C()
{
	SIM->CLKDIV1 = SIM->CLKDIV1 | (1 << 17) | (1 << 16); // bus clock is 24/3 = 8Hz
	SIM->SCGC5 = SIM->SCGC5 | SIM_SCGC5_PORTE_MASK; //clock to PTE24 and PTE25 for I2C0
	PORTE->PCR[24] |=  5 << 8;											// set PORTE 24 I2C
	PORTE->PCR[25] |=  5 << 8;											// set PORTE 24 I2C
	
	SIM->SCGC4 |= 1 << 6; // enable clock to I2C0
	I2C0->F = 0x1F; // set I2C baud rate 
	I2C0->C1 |= 1 << 7; // enable I2C
	//I2C0->C1 |= 1 << 6; // enable I2C interrupt
	//I2C0->C1 &= 0 << 4; // select recieve mode;
}

void writeI2C(uint8_t data)
{
	// when writing to the I2C, I2C0_S TFC = 0
	//while (!(I2C0->S & 0x80));
	I2C0->D = data;
}
void transmit(uint8_t reg_add, uint8_t data)
{
	// start signal : I2C0_C1 MST bit(0-1)
	//I2C0->C1 &= 0 << 5;
	I2C0->C1 |= 1 << 5;
	
	// Select transmit mode
	I2C0->C1 |= 1 << 4;
	
	// send MMA address + bit W
	writeI2C(0x1D << 1);
	
	// wait for interrupt of ACK/NACK is set
	while ((I2C0->S & 0x02) == 0);
	I2C0->S |= 0x02; // write to clear interrupt
	
	// check bit ACKRX ....
	if (!(I2C0->S & 1)) {}
	
	// send register address 
	writeI2C(reg_add);
	
	// wait for interrupt of ACK/NACK is set
	while (!(I2C0->S & 0x02));
	I2C0->S |= 0x02; // write to clear interrupt 
	
	// check bit ACKRX ....
	if (!(I2C0->S & 1)) {}
		
	// send data
	writeI2C(data);
	
	// wait for interrupt of ACK/NACK is set
	while (!(I2C0->S & 0x02));
	I2C0->S |= 0x02; // write to clear interrupt

	// check bit ACKRX ....
	if (!(I2C0->S & 1)) {}
	
	// stop signal: I2C0_C1 MST bit(0-1)
	I2C0->C1 &= ~(1 << 5);
	
	// receive mode
	I2C0->C1 &= ~(1 << 4);
	
	// delay
	for (int i = 0; i < 1000; i++);
}

uint8_t receive(uint8_t reg_add)
{
		// start signal : I2C0_C1 MST bit(0-1)
	I2C0->C1 |= 1 << 5;
	
	// transmit mode
	I2C0->C1 |= 1 << 4;
	
	// send MMA address + bit W
	writeI2C(0x1D << 1);
	
	// wait for interrupt of ACK/NACK is set
	while (!(I2C0->S & 0x02));
	I2C0->S |= 0x02; // set IICIF 
	
	// check bit ACKRX ....
	if (!(I2C0->S & 1)) {}
	
	// send register address 
	writeI2C(reg_add);
	
	// wait for interrupt of ACK/NACK is set
	while (!(I2C0->S & 0x02));
	I2C0->S |= 0x02; // set IICIF 
	
	// check bit ACKRX ....
	if (!(I2C0->S & 1)) {}
	
	// repeated start signal
	I2C0->C1 |= 1 << 2;
	
	// send MMA address + bit R
	writeI2C((0x1D << 1) | 0x01);
	
	// wait for interrupt of ACK/NACK is set
	while (!(I2C0->S & 0x02));
	I2C0->S |= 0x02; // set IICIF 
	
	// check bit ACKRX ....
	if (!(I2C0->S & 1)) {}
	
	// recieve mode + send NACK
	I2C0->C1 &= ~(1 << 4);							// recieve mode
	I2C0->C1 |= 1 << 3; 							// send NAK
	
	// read dummy
	uint8_t data;
	data = I2C0->D;

	// wait for interrupt
	while (!(I2C0->S & 0x02));
	I2C0->S |= 0x02; // set IICIF 
	
	// check bit ACKRX ....
	if (!(I2C0->S & 1)) {}
	
	// stop signal: I2C0_C1 MST bit(0-1)
	I2C0->C1 &= ~(1 << 5);
	
	data = I2C0->D;
	// delay
	for (int i = 0; i < 1000; i++);
	
	return data;
}

void initINT2()
{
	SIM->SCGC5 |= 1 << 12;												//enables clock to port D
 	PORTD->PCR[1] |= (1 << 8);				//set PTC3 to GPIO
	PTD->PDDR &= ~((uint32_t)1 << 1); 													//set PTC3 as an input
	PORTD->PCR[1] |= (0xA) << 16; 								//select falling edge interrupts for PORTC[3]
	NVIC_ClearPendingIRQ(31);											//clear NVIC any pending interrupts on PORTC_D
	NVIC_EnableIRQ(31);														//enable NVIC interrupts source for PORTC_D
	
}

void configMMA84512Q()
{
	transmit(0x2A, 0x20); // put the device in Standby Mode => 50 Hz ODR
	transmit(0x15, 0xB8);	// enable latch, freefall, X=axis, Y-axis, Z-axis
	transmit(0x17, 0x03); // set threshold 0.2g
	transmit(0x18, 0x06); // 120 ms debounce timing (50Hz sample rate; 120ms/20ms = 6 counts) Normal Mode 
	transmit(0x2D, 0x04);	// enable interrupt signal
	transmit(0x2E, 0x00);	// choose output interrupt signal INT2 : PTD1
	uint8_t CTRL_REG1_Data = receive(0x2A);
	CTRL_REG1_Data |= 0x01;
	transmit(0x2A, CTRL_REG1_Data);
}

