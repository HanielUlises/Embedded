#include "i2c.h"

#include "stm32f4xx.h"

#define GPOIBEN 					(1U << 1)
#define I2C1EN						(1U << 21)

#define I2C_100KHZ					80
#define SD_MODE_MAX_RISE_TIME		17
#define CR1_PE						(1U << 0)

// PB8 ------ SCL
// PB9 ------ SDA

void I2C1_init(void) {
	// Enable clock access to IC2C1
	RCC -> AHB1ENR |= GPIOBEN;
	// Set PB8 and PB9 mode to alternate function
	GPIOB -> MODER &= ~(1U << 16);
	GPIOB -> MODER |= (1U << 17);

	GPIOB -> MODER &= ~(1U << 18);
	GPIOB -> MODER |= (1U << 19);

	// Set PB8 and PB9 output type to open
	GPIOB -> OTYPER |= (1U << 8);
	GPIOB -> OTYPER |= (1U << 9);
	// Enable pull-up for PB8 and PB9
	GPIOB -> PUPDR |= (1U << 16);
	GPIOB -> PUPDR &= ~(1U << 17);
	GPIOB -> PUPDR |= (1U << 18);
	GPIOB -> PUPDR &= ~(1U << 19);

	// Enable clock access to I2C1
	RCC -> APB1ENR |= I2C1EN;

	// Enter reset mode
	I2C1 -> CR1 |= (1U << 15);

	// Come out of reset mode
	I2C1 -> CR1 &= ~(1U << 15);

	// Set peripheral clock frequency
	I2C1 -> CR2 = (1U << 4);			// 16mhz

	// Set I2C to standard mode, 100kHz clock
	I2C1 -> CCR = I2C_100KHZ;

	// Set rise time
	I2C1 -> TRISE = SD_MODE_MAX_RISE_TIME;

	// Enable I2C1 module
	I2C1 -> CR1 |= CR1_PE;
}
