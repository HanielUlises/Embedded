#include <stdio.h>

#include "uart.h"
#include "adc.h"
#include "systick.h"
#include "tim.h"
#include "exti.h"

uint32_t sensor_value;

#define GPIOAEN 				(1U << 0)
#define GPIOA_5					(1U << 5)

#define PIN5					(1U << 5)

#define LED_PIN					PIN5

int timestamp = 0;
char key;

static void dma_callback(void);

void DMA1_Stream6_IRQHandler(void);

int main(void) {
	char message[31] = "[Log] STM32 DMA Transfer\n\r";
	// Enable clock access to GPIOA
	RCC -> AHB1ENR |= GPIOAEN;

	// Set PA5 as output pin
	GPIOA -> MODER |= (1U << 10);
	GPIOA -> MODER &= ~(1U << 11);

	uart2_tx_init();
	dma1_stream6_init((uint32_t) message, (uint32_t) &USART2 -> DR, 31);

	while(1) {

	}
    return 0;
}

static void dma_callback(void) {
	GPIOA -> ODR |= LED_PIN;
}

void DMA1_Stream6_IRQHandler(void) {
	if(DMA1 -> HISR & HISR_TCIF6) {
		// CLear flag
		DMA1 -> HIFCR |= HIFCR_CTCFI6;
		// Do something
	}
}
