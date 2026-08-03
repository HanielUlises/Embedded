#include <stdio.h>

#include "uart.h"
#include "adc.h"

uint32_t sensor_value;

int main(void) {


	uart2_tx_init();
	pa1_adc_init();

	while(1) {
		start_conversion();
		sensor_value = adc_read();
		printf("Sensor value: %li \n\r", sensor_value);
	}
}
