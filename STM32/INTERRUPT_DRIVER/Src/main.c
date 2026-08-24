#include <stdio.h>

#include "uart.h"
#include "adc.h"
#include "systick.h"
#include "tim.h"
#include "exti.h"

uint32_t sensor_value;

#define GPIOAEN 				(1U << 0)
#define PIN5					(1U << 5)

#define LED_PIN					PIN5

int timestamp = 0;

static void exti_callback(void);

int main(void) {

	pc13_exti_init();
	uart2_tx_init();

	while(1) {

	}

}

static void exti_callback(void) {
	printf("BTN Pressed...\n\r");
}

void EXTI15_10_IRQHandler(void) {
	if((EXTI -> PR & LINE13) != 0) {
		// Clear the PR flag
		EXTI -> PR |= LINE13;
		exti_callback();
	}
}

