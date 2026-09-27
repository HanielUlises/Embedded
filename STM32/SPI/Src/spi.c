#include "spi.h"

#define SPI1EN 				(1U << 12)
#define GPIOAEN				(1U << 0)

// PA5 -> CLK
// PA6 -> MISO
// PA7 -> MOSI

// PA9 -> Slave select

void spi_gpio_init(void) {
	// Enable clock access to GPIOA
	RCC -> AHB1ENR |= GPIOAEN;

	// Set PA5, PA6, PA7 mode to alternate function
	// PA5
	GPIOA -> MODER &= ~(1U << 10);
	GPIOA -> MODER |= (1U << 11);

	// PA6
	GPIOA -> MODER &= ~(1U << 12);
	GPIOA -> MODER |= (1U << 13);

	// PA7
	GPIOA -> MODER &= ~(1U << 14);
	GPIOA -> MODER |= (1U << 15);

	// Set PA5, PA6, PA7 alternate function type to SPI1
	// PA5
	GPIOA -> AFR[0] |= (1U << 20);
	GPIOA -> AFR[0] &= ~(1U << 21);
	GPIOA -> AFR[0] |= (1U << 22);
	GPIOA -> AFR[0] &= ~(1U << 23);

	// PA6
	GPIOA -> AFR[0] |= (1U << 24);
	GPIOA -> AFR[0] &= ~(1U << 25);
	GPIOA -> AFR[0] |= (1U << 26);
	GPIOA -> AFR[0] &= ~(1U << 27);

	// PA7
	GPIOA -> AFR[0] |= (1U << 28);
	GPIOA -> AFR[0] &= ~(1U << 29);
	GPIOA -> AFR[0] |= (1U << 30);
	GPIOA -> AFR[0] &= ~(1U << 31);
}

void spi1_config(void) {
	// Enable clock access to SPI1 module
	RCC -> APB2ENR |= SPI1EN;

	// Set clock to fPCLK/4
	SPI1 -> CR1 |= (1U << 3);
	SPI1 -> CR1 &= ~(1U << 4);
	SPI1 -> CR1 &= ~(1U << 5);

	// Set CPOL to 1 and CPHA to 1
	SPI1 -> CR1 |= (1U << 0);
	SPI1 -> CR1 |= (1U << 1);

	// Enable full duplex
	SPI1 -> CR1 &= ~(1U << 10);

	// Set MSB first
	SPI1 -> CR1 &= ~(1U << 7);

	// Set mode to MASTER
	SPI1 -> CR1 |= (1U << 2);

	// Set 8 bit data mode
	SPI1 -> CR1 &= ~(1U << 11);

	// Select software slave management by
	// setting SSM = 1 and SSI = 1
	SPI1 -> CR1 |= (1U << 8);
	SPI1 -> CR1 |= (1U << 9);
}

void spi1_transmit(uint8_t *data, uint32_t size) {
	uint32_t i = 0;
	uint32_t temp;

	while(i < size) {
		// Wait until TXE is set
		while(!(SPI1 -> SR & (SR_TXE))) {}
		// Write the data to the data register
		SPI1 -> DR = data[i];
		i++;
	}
	// Wait until TXE is set
	while(!(SPI1 -> SR & (SR_TXE))) {}
	// Wait for BUSY flag to reset
	while((SPI1 -> SR & (SR_BUSY))) {}

	// Clear OVR flag
	temp = SPI1 -> DR;
	temp = SPI1 -> SR;

}
