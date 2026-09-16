#include "adxl345.h"

char data;

uint8_t data_rec[6];

int adxl_read_address(uint8_t reg, char *out) {
	return I2C1_byte_read(DEVICE_ADDR, reg, out);
}

static int adxl_write(uint8_t reg, char value) {
	char buf[1];
	buf[0] = value;

	return I2C1_burst_write(DEVICE_ADDR, reg, 1, buf);
}

int adxl_read_values(uint8_t reg) {
	return I2C1_burst_read(DEVICE_ADDR, reg, 6, (char*) data_rec);
}

int adxl_init(void) {
	int rc;

	// Enable I2C
	I2C1_init();

	// Read the DEVID, this should return 0xE5
	rc = adxl_read_address(DEVID_R, &data);
	if(rc != I2C_OK) { return rc; }
	if((unsigned char) data != ADXL_DEVID) { return I2C_ERR_NACK; }

	// Set data format range to +-4g
	rc = adxl_write(DATA_FORMAT_R, FOUR_G);
	if(rc != I2C_OK) { return rc; }

	// Reset all bits
	rc = adxl_write(POWER_CTL_R, RESET);
	if(rc != I2C_OK) { return rc; }

	// Configure power control measure bit
	return adxl_write(POWER_CTL_R, SET_MEASURE_B);
}
