#include <stdio.h>

#include "uart.h"
#include "adc.h"
#include "systick.h"

uint32_t sensor_value;

#define GPIOAEN 				(1U << 0)
#define PIN5					(1U << 5)

#define LED_PIN					PIN5

int main(void) {
	RCC -> AHB1ENR |= GPIOAEN;
	GPIOA -> MODER |= (1U << 10);
	GPIOA -> MODER &=~ (1U << 11);

	uart2_tx_init();

	while(1) {
		printf("[Log| A second passed]");
		systick_delayMs(1000);
	}
}
