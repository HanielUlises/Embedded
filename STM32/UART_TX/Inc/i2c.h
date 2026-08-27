/*
 * i2c.h
 *
 *  Created on: Aug 27, 2026
 *      Author: haniel
 */

void I2C1_init(void);
void I2C1_byte_read(char saddr, char maddr, char *data);
