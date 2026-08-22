#include <stdio.h>

#include "uart.h"

#define GIPOAEN 		(1U << 0)
#define GPIOA_5			(1U << 5)

#define LED_PIN			GPIOA_5

char key;

int main(void) {
	// Enable clock access to GPIOA
	RCC -> AHB1ENR |= GPIOAEN;
	// Set PA5 as output pin
	GPIOA -> MODER |= (1U<<10);
	GPIOA -> MODER &= ~(1U<<10);
	uart2_rxtx_init();
	while(1) {
		key = uart2_read();
		if(key == '1') {
				GPIOA -> ODR |= LED_PIN;
		} else {
			GPIO -> ODR &= ~LED_PIN;
		}
	}
}
