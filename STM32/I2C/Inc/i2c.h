#ifndef I2C_H
#define I2C_H

void I2C1_init(void);
void I2C1_byte_read(char saddr, char maddr, char *data);
void I2C1_burst_read(char saddr, char maddr, int n, char *data);
void I2C1_burst_write(char saddr, char maddr, int n, char *data);

#endif
