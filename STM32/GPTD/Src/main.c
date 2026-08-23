#include <stdio.h>

#include "uart.h"
#include "adc.h"
#include "systick.h"
#include "tim.h"

uint32_t sensor_value;

/*
#define GPIOAEN 				(1U << 0)
#define PIN5					(1U << 5)

#define LED_PIN					PIN5 */

int timestamp = 0;

int main(void) {
	tim2_pa5_output_compare();
	tim3_pa6_input_capture();

	while(1) {
		// Wait until edge is captured
		while(!(TIM3 -> SR  & SR_CC1IF)) {}
		// Read captured value
		timestamp = TIM3 -> CCR1;
	}
	/*
	RCC -> AHB1ENR |= GPIOAEN;
	GPIOA -> MODER |= (1U << 10);
	GPIOA -> MODER &=~ (1U << 11);

	uart2_tx_init();
	tim2_1hz_init();

	while(1) {
		// Wait for UIF
		while(!(TIM2 -> SR & SR_UIF)) {}

		TIM2 -> SR &= ~SR_UIF;
		printf("[Log| A second passed]");
		GPIOA -> ODR ^= LED_PIN;
		systick_delayMs(1000);
	}
	*/


}
