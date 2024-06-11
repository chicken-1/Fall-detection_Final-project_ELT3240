#include "MKL46Z4.h"
#include "I2C.h"

void InitI2C()
{
	SIM->CLKDIV1 = SIM->CLKDIV1 | (1 << 17) | (1 << 16); // bus clock is 24/3 = 8Mz
	SIM->SCGC5 = SIM->SCGC5 | SIM_SCGC5_PORTE_MASK; //clock to PTE24 and PTE25 for I2C0
	SIM->SCGC4 |= 1 << 6; // enable clock to I2C0
	I2C0->F = 0x07; // set I2C baud rate 25kbits/s
	I2C0->C1 |= 1 << 7; // enable I2C
	I2C0->C1 |= 1 << 6; // enable I2C interrupt
	I2C0->C1 &= 0 << 4; // select recieve mode;
	I2C0->C1 |= 1 << 5; //change MST bit from 0 to 1 -> generate START signal & select master mode
}

void writeI2C(uint8_t data)
{
	// when writing to the I2C, I2C0_S TFC = 0
	while (!(I2C0->S & 0x80));
	I2C0->D = data;
}
void transmit(uint8_t reg_add, uint8_t data)
{
	// start signal : I2C0_C1 MST bit(0-1)
	I2C0->C1 &= 0 << 5;
	I2C0->C1 |= 1 << 5;
	
	// send MMA address + bit W
	writeI2C(0x16 << 1);
	
	// wait for interrupt of ACK/NACK is set
	while (!(I2C0->S & 0x02));
	// I2C0->S |= 0x02; // set IICIF 
	
	// send register address 
	writeI2C(reg_add);
	
	// wait for interrupt of ACK/NACK is set
	while (!(I2C0->S & 0x02));
	// I2C0->S |= 0x02; // set IICIF 
	
	// send data
	writeI2C(data);
	
	// wait for interrupt of ACK/NACK is set
	while (!(I2C0->S & 0x02));
	// I2C0->S |= 0x02; // set IICIF 
	
	// stop signal: I2C0_C1 MST bit(0-1)
	I2C0->C1 |= 1 << 5;
	I2C0->C1 &= 0 << 5;
	
	// delay
	for (int i = 0; i < 1000; i++);
}

uint8_t recieve(uint8_t reg_add)
{
		// start signal : I2C0_C1 MST bit(0-1)
	I2C0->C1 &= 0 << 5;
	I2C0->C1 |= 1 << 5;
	
	// send MMA address + bit W
	writeI2C(0x16 << 1);
	
	// wait for interrupt of ACK/NACK is set
	while (!(I2C0->S & 0x02));
	// I2C0->S |= 0x02; // set IICIF 
	
	// send register address 
	writeI2C(reg_add);
	
	// wait for interrupt of ACK/NACK is set
	while (!(I2C0->S & 0x02));
	// I2C0->S |= 0x02; // set IICIF 
	
	// repeated start signal
	I2C0->C1 |= 1 << 2;
	
	// send MMA address + bit R
	writeI2C((0x16 << 1) | 0x01);
	
	// wait for interrupt of ACK/NACK is set
	while (!(I2C0->S & 0x02));
	// I2C0->S |= 0x02; // set IICIF 
	
	// read data and send NAK signal
	I2C0->SMB |= 1 << 7; // set bit FACK to generate a NACK
	while (!(I2C0->C1 & (1 << 3))); // wait for bit TXAK is set
	uint8_t data = I2C0->D;	
	
	// stop signal: I2C0_C1 MST bit(0-1)
	I2C0->C1 |= 1 << 5;
	I2C0->C1 &= 0 << 5;
	
	// delay
	for (int i = 0; i < 1000; i++);
	
	return data;
}

void configMMA84512Q()
{
	transmit(0x2A, 0x20); // active mma
	transmit(0x15, 0xB8);
	transmit(0x17, 0x03);
	transmit(0x18, 0x06);
	transmit(0x2D, 0x04);
	transmit(0x2E, 0x00);
	uint8_t CTRL_REG1_Data = recieve(0x2A);
	CTRL_REG1_Data |= 0x01;
	transmit(0x2A, CTRL_REG1_Data);
}

