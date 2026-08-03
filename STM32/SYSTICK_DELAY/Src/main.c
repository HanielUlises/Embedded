#include <stdio.h>

#include "uart.h"
#include "adc.h"
#include "systick.h"

uint32_t sensor_value;

int main(void) {


	uart2_tx_init();
	pa1_adc_init();
	start_conversion();
	while(1) {

		sensor_value = adc_read();
		printf("Sensor value: %li \n\r", sensor_value);
	}
}
