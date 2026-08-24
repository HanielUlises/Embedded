#include <stdio.h>

#include "uart.h"
#include "adc.h"
#include "systick.h"
#include "tim.h"
#include "exti.h"

// #define UART
#define ADC

uint32_t sensor_value;

#define GPIOAEN 				(1U << 0)
#define GPIOA_5					(1U << 5)

#define PIN5					(1U << 5)

#define LED_PIN					PIN5

int timestamp = 0;
char key;

static void exti_callback(void);
static void uart_callback(void);

void USART2_IRQHandler(void);

int main(void) {
#ifdef UART

    // Enable GPIOA clock
    RCC->AHB1ENR |= GPIOAEN;

    // Configure PA5 as output
    GPIOA->MODER |= (1U << 10);
    GPIOA->MODER &= ~(1U << 11);

    // Initialize UART2 RX interrupt
    uart2_rx_interrupt_init();

    while (1)
    {
        // Main loop
    }

#elif defined(ADC)

    // ADC initialization goes here
    uart2_tx_init();
    pa1_adc_interrupt_init();
    start_conversion();

    while (1)
    {
        sensor_value = adc_read();
        printf("Sensor value: %d \n\r", (int) sensor_value);
    }

#endif

    return 0;
}

static void uart_callback(void) {
	key = USART2 -> DR;
	if(key == '1') {
			GPIOA -> ODR |= LED_PIN;
	} else {
			GPIOA -> ODR &= ~LED_PIN;
	}
}

void USART2_IRQHandler(void) {
	// RXNE set (check)
	if(USART2 -> SR & SR_RXNE) {
		uart_callback();
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

