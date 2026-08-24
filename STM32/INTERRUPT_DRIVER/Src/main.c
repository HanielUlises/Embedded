#include <stdio.h>

#include "uart.h"
#include "adc.h"
#include "systick.h"
#include "tim.h"
#include "exti.h"

// #define UART_INTER_DRIVER
// #define ADC_INTER_DRIVER
// #define SYSTICK_INTER_DRIVER
#define TIMER_INTER_DRIVER

uint32_t sensor_value;

#define GPIOAEN 				(1U << 0)
#define GPIOA_5					(1U << 5)

#define PIN5					(1U << 5)

#define LED_PIN					PIN5

int timestamp = 0;
char key;

static void adc_callback(void);
static void exti_callback(void);
static void systick_callback(void);
static void uart_callback(void);


void ADC_IRQHandler(void);
void EXTI15_10_IRQHandler(void);
void SysTick_IRQHandler(void);
void USART2_IRQHandler(void);



int main(void) {
#ifdef UART_INTER_DRIVER

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

#elif defined(ADC_INTER_DRIVER)

    // ADC initialization goes here
    uart2_tx_init();
    pa1_adc_interrupt_init();
    start_conversion();

    while (1)
    {

    }

#elif defined(SYSTICK_INTER_DRIVER)
    // Enable clock access to GPIOA
    RCC -> APB1ENR |= GPIOAEN;

    // Set PA5 as output pin
    GPIOA -> MODER |= (1U << 10);
    GPIOA -> MODER &= ~(1U << 11);

    uart2_tx_init();
    systick_1hz_interrupt();

    while(1) {

    }

#elif defined(TIMER_INTER_DRIVER)
    // Enable clock access to GPIOA
    RCC -> APB1ENR |= GPIOAEN;

    // Set PA5 as output pin
    GPIOA -> MODER |= (1U << 10);
    GPIOA -> MODER &= ~(1U << 11);

    uart2_tx_init();
    tim2_1hz_interrupt_init();

    while(1) {

    }

#endif

    return 0;
}


static void adc_callback(void) {
	sensor_value = ADC1 -> DR;
	printf("Sensor value: %d \n\r", (int) sensor_value);
}

static void exti_callback(void) {
	printf("BTN Pressed...\n\r");
}

static void systick_callback(void) {
	printf("A second has passed! \n\r");
	GPIOA -> ODR ^= LED_PIN;
	systick_delayMs(1000);
}

static void tim2_callback(void) {
	printf("A second has passed\n\r");
	GPIOA -> ODR ^= LED_PIN;
}

static void uart_callback(void) {
	key = USART2 -> DR;
	if(key == '1') {
			GPIOA -> ODR |= LED_PIN;
	} else {
			GPIOA -> ODR &= ~LED_PIN;
	}
}

void ADC_IRQHandler(void) {
	// Check for EOC in SR
	if((ADC1 -> SR & SR_EOC) != 0) {
		// Clear EOC
		ADC1 -> SR &= ~SR_EOC;
		adc_callback();
	}
}

void EXTI15_10_IRQHandler(void) {
	if((EXTI -> PR & LINE13) != 0) {
		// Clear the PR flag
		EXTI -> PR |= LINE13;
		exti_callback();
	}
}

void SysTick_IRQHandler(void) {
	systick_callback();
}

void TIM2_IRQHandler(void) {
	// Clear update interrupt flag
	TIM2 -> SR &= ~SR_UIF;

}

void USART2_IRQHandler(void) {
	// RXNE set (check)
	if(USART2 -> SR & SR_RXNE) {
		uart_callback();
	}
}



