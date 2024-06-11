#include "MKL46Z4.h"

void InitI2C(void);
void writeI2C(uint8_t data);
void configMMA84512Q(void);
void transmit(uint8_t reg_add, uint8_t data);
uint8_t recieve(uint8_t reg_add);
