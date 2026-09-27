#ifndef ADXL345_H
#define ADXL345_H

#include "i2c.h"
#include <stdint.h>

#define DEVID_R					(0x00)
#define DEVICE_ADDR				(0x53)
#define DATA_FORMAT_R			(0x31)
#define POWER_CTL_R				(0x2D)

#define DATA_START_ADDR			(0X32)

#define FOUR_G					(0x01)
#define RESET					(0x00)
#define SET_MEASURE_B			(0x08) // 0b 1000

#define ADXL_DEVID				(0xE5)	/* what DEVID_R must return */

/* Returns I2C_OK once the device answered with the right DEVID, else the
 * I2C error code, or I2C_ERR_NACK-style failure from the transfer. */
int adxl_init(void);
int adxl_read_address(uint8_t reg, char *out);
int adxl_read_values(uint8_t reg);

#endif
