#include "uart.h"

#define GPIOAEN 		(1U << 0)
#define UART2EN			(1U << 17)

#define CR1_TE			(1U << 3)
#define CR1_RE			(1U << 2)

#define CR1_UE			(1U << 13)
#define SR_TXE			(1U << 7)
#define SR_RXNE			(1U << 5)
#define CR1_RXNEIE		(1U << 5)

#define SYS_FREQ		16000000
#define APB1_CLK		SYS_FREQ

#define UART_BAUDRATE	115200

static uint16_t compute_uart_bd(uint32_t PeriphClk, uint32_t BaudRate) {
	return ((PeriphClk + (BaudRate/2U)) / BaudRate);
}

static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate) {
	USARTx -> BRR = compute_uart_bd(PeriphClk, BaudRate);
}

void uart2_write(int ch) {
	// Make sure the transmit data register is empty
	while(!(USART2 -> SR & SR_TXE)){}
	// Write to transmit data register
	USART2 -> DR = (ch & 0xFF);
}

void uart2_rx_interrupt_init(void) {
    /* Enable clock access to GPIOA */
    RCC->AHB1ENR |= GPIOAEN;

    // Set PA2 mode to alternate function mode
    GPIOA->MODER &= ~(1U << 4);
    GPIOA->MODER |=  (1U << 5);

    // Set PA2 alternate function type to UART_TX (AF07)
	GPIOA -> AFR[0] |= (1U << 8);
	GPIOA -> AFR[0] |= (1U << 9);
	GPIOA -> AFR[0] |= (1U << 10);
	GPIOA -> AFR[0] &= ~(1U << 11);

	// Set PA3 mode to alternate function mode
	GPIOA -> MODER &= ~(1U << 6);
	GPIOA -> MODER &= (1U << 7);

	/*************Configure UART module ******************/
	// Enable clock access to UART2
	RCC -> APB1ENR |= UART2EN;
	// Configure baudrate
	uart_set_baudrate(USART2, APB1_CLK, UART_BAUDRATE);
	// Configure the transfer direction transmission and receiver
	USART -> CR1 |= CR1_RE;
	// Enable RXNE interrupt
	USART -> CR1 |= CR1__RXNEIE;
	// Enable UART2 interrupt in NVIC
	NVIC_EnableIRQ(USART2_IRQn);
	// Enable UART module
	USART2 -> CR1 |= CR1_UE;

}

void uart2_tx_init(void) {
    /************ Configure PA2 as UART2_TX ************/

    /* Enable clock access to GPIOA */
    RCC->AHB1ENR |= GPIOAEN;

    /* Set PA2 to Alternate Function mode */
    GPIOA->MODER &= ~(3U << 4);
    GPIOA->MODER |=  (2U << 4);

    /* Select AF7 (USART2_TX) for PA2 */
    GPIOA->AFR[0] &= ~(0xFU << 8);
    GPIOA->AFR[0] |=  (7U << 8);

    /************ Configure USART2 ************/

    /* Enable clock access to USART2 */
    RCC->APB1ENR |= UART2EN;

    /* Configure baud rate */
    uart_set_baudrate(USART2, APB1_CLK, UART_BAUDRATE);

    /* Enable transmitter only */
    USART2->CR1 = CR1_TE;

    /* Enable USART2 */
    USART2->CR1 |= CR1_UE;
}

void uart2_rxtx_interrupt_init(void) {
	/************Configure UART GPIO pin*************/
	// Enable clock access to GPIOA
	RCC -> AHB1ENR |= GPIOAEN;
	// set PA2 mode to alternate function mode
	GPIOA -> MODER &= ~(3U << 4);
	GPIOA -> MODER |=  (2U << 4);

	// set PA2 alternate function type to UART_TX (AF07)
	GPIOA -> AFR[0] |= (1U << 8);
	GPIOA -> AFR[0] |= (1U << 9);
	GPIOA -> AFR[0] |= (1U << 10);
	GPIOA -> AFR[0] &= ~(1U << 11);

	// set PA3 mode to alternate function mode
	GPIOA -> MODER &=~ (1U<<6);
	GPIOA -> MODER |=  (1U<<7);

	// set PA3 mode type to UART_RX (AF07)
	GPIOA -> AFR[0] |= (1U<<12);
	GPIOA -> AFR[0] |= (1U<<13);
	GPIOA -> AFR[0] |= (1U<<14);
	GPIOA -> AFR[0] &= ~(1U<<14);

	// Enable clock access to uart2
	RCC -> APB1ENR |= UART2EN;

	// Configure baudrate
	uart_set_baudrate(USART2, APB1_CLK, UART_BAUDRATE);
	// Configure the transfer direction
	USART2 -> CR1 = (CR1_TE | CR1_RE);
	// Enable RXNE interrupt
	USART2 -> CR1 |= CRCR1_RXNEIE;
	// Enable UART2 interrupt in NVIC
	NVIC_EnaleIRQ(USART2_IRQn);
	// Enable UART module
	USART2 -> CR1 |= CR1_UE;
}

char uart2_read(void) {
	// Make sure the receive data register is not empty
	while(!(USART2 -> SR & SR_RXNE)) {}
	return USART2 -> DR;
}

int __io_putchar(int ch) {
	uart2_write(ch);
	return ch;
}
