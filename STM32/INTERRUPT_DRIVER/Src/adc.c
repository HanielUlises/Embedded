#include "adc.h"
#include "stm32f4xx.h"

#define GPIOAEN 			(1U << 0)
#define ADC1EN 				(1U << 8)
#define ADC_CH1 			(1U << 0)
#define ADC_SEQ_LEN_1		0x00
#define CR2_ADON			(1U << 0)
#define CR2_SWSTART			(1U << 30)
#define SR_EOC				(1U << 1)
#define CR2_CONT			(1U << 1)
#define CR1_EOCIE			(1U << 5)

void pa1_adc_interrupt_init(void) {
	// Configure the ADC GPIO pin

	// Enable clock access to GPIOA
	RCC -> AHB1ENR |= GPIOAEN;
	// Set the mode of PA1 to analog
	GPIOA -> MODER |= (1U << 2);
	GPIOA -> MODER |= (1U << 3);
	// Configure ADC module
	// Enable clock access to ADC
	RCC -> APB2ENR |= ADC1EN;

	// Enable ADC end-of-conversion interrupt
	ADC1 -> CR1 |= CR1_EOCIE;

	// Enable ADC interrupt in NVIC
	NVIC_EnableIRQ(ADC_IRQn);

	// ADC parameters
	// Configure conversion sequence start
	ADC1 -> SQR3 = ADC_CH1;
	// Conversion sequence length
	ADC1 -> SQR1 =ADC_SEQ_LEN_1;
	// Enable ADC module
	ADC1 -> CR2 |= CR2_ADON;
}

void start_conversion(void) {
	// Continuous conversion
	ADC1 -> CR2 |= CR2_CONT;
	// Start ADC conversion
	ADC1 -> CR2 |= CR2_SWSTART;
}

uint32_t adc_read(void) {
	// Wait for conversion to be complete
	while(!(ADC1 -> DR & SR_EOC)) {}
	// Read converted result
	return (ADC1 -> DR);

}
