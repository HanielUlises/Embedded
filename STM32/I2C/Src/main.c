#include <stdio.h>
#include <stdint.h>

#include "stm32f4xx.h"
#include "uart.h"
#include "adxl345.h"

int16_t x,y,z;
float xg, yg, zg;

extern uint8_t data_rec[6];
extern char data;

int main(void) {
	uart2_tx_init();
	setvbuf(stdout, NULL, _IONBF, 0);
	printf("boot\r\n");

	/* ADXL345 CS on D8 (PA9): hold high to select I2C mode */
	RCC->AHB1ENR |= (1U << 0);
	GPIOA->MODER &= ~(3U << 18);
	GPIOA->MODER |=  (1U << 18);
	GPIOA->ODR   |=  (1U << 9);

	int rc = adxl_init();
	printf("adxl init rc=%d devid=0x%02X (expected 0x%02X)\r\n",
			rc, (unsigned char) data, ADXL_DEVID);

	if(rc != I2C_OK) {
		/* Stop here rather than streaming garbage: the rc tells you which
		 * stage failed, so check wiring/pull-ups/SDO before going further. */
		printf("adxl not responding, halting\r\n");
		while(1) {}
	}

	while(1) {
		rc = adxl_read_values(DATA_START_ADDR);
		if(rc != I2C_OK) {
			printf("read failed rc=%d\r\n", rc);
			continue;
		}

		x = ((data_rec[1] << 8) | data_rec[0]);
		y = ((data_rec[3] << 8) | data_rec[2]);
		z = ((data_rec[5] << 8) | data_rec[4]);

		xg = x *  0.0078;
		yg = y *  0.0078;
		zg = z *  0.0078;

		/* printf %f is off with nano.specs, so print milli-g as integers */
		printf("x=%6d y=%6d z=%6d | mg: %6d %6d %6d\r\n",
				x, y, z, (int)(xg * 1000), (int)(yg * 1000), (int)(zg * 1000));

	}
}
