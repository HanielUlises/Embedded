#include "i2c.h"

#include "stm32f4xx.h"

#define GPIOBEN 					(1U << 1)
#define I2C1EN						(1U << 21)

#define I2C_100KHZ					80
#define SD_MODE_MAX_RISE_TIME		17
#define CR1_PE						(1U << 0)

#define SR2_BUSY					(1U << 1)
#define CR1_START					(1U << 8)
#define SR1_SB						(1U << 0)
#define SR1_ADDR					(1U << 1)
#define SR1_TXE						(1U << 7)
#define CR1_ACK						(1U << 10)
#define CR1_STOP					(1U << 9)
#define SR1_RXNE					(1U << 6)
#define SR1_BTF						(1U << 2)
#define SR1_AF						(1U << 10)	/* acknowledge failure */

/* Loops, not microseconds. At 16MHz this is a few tens of ms, which is
 * ages compared to a 100kHz byte transfer but still quick to fail. */
#define I2C_TIMEOUT					100000

/* Abort: stop the transfer so the bus is left usable for the next attempt. */
static int i2c_abort(int err) {
	I2C1->CR1 |= CR1_STOP;
	return err;
}

/* Spin until the flag in SR1 is set, giving up after I2C_TIMEOUT. */
#define WAIT_SR1(flag, err)										\
	do {														\
		unsigned int _t = I2C_TIMEOUT;							\
		while(!(I2C1->SR1 & (flag))) {							\
			if(--_t == 0U) { return i2c_abort(err); }			\
		}														\
	} while(0)

/* The addr phase is the one that detects a missing device: a NACK sets AF
 * instead of ADDR, so watch both or a dead slave just burns the timeout. */
#define WAIT_ADDR()												\
	do {														\
		unsigned int _t = I2C_TIMEOUT;							\
		while(!(I2C1->SR1 & SR1_ADDR)) {						\
			if(I2C1->SR1 & SR1_AF) {							\
				I2C1->SR1 &= ~SR1_AF;							\
				return i2c_abort(I2C_ERR_NACK);					\
			}													\
			if(--_t == 0U) { return i2c_abort(I2C_ERR_ADDR); }	\
		}														\
	} while(0)

#define WAIT_NOT_BUSY()											\
	do {														\
		unsigned int _t = I2C_TIMEOUT;							\
		while(I2C1->SR2 & SR2_BUSY) {							\
			if(--_t == 0U) { return I2C_ERR_BUSY; }				\
		}														\
	} while(0)

// PB8 ------ SCL
// PB9 ------ SDA

void I2C1_init(void) {
	// Enable clock access to GPIOB
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

	// Set PB8 and PB9 alternate function type to I2C (AF4)
	GPIOB -> AFR[1] &=~(1U << 0);
	GPIOB -> AFR[1] &=~(1U << 1);
	GPIOB -> AFR[1] |= (1U << 2);
	GPIOB -> AFR[1] &=~(1U << 3);

	GPIOB -> AFR[1] &=~(1U << 4);
	GPIOB -> AFR[1] &=~(1U << 5);
	GPIOB -> AFR[1] |= (1U << 6);
	GPIOB -> AFR[1] &=~(1U << 7);

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

int I2C1_byte_read(char saddr, char maddr, char *data) {
	volatile int temp;
	// Wait until bus not busy
	WAIT_NOT_BUSY();

	// Generate start
	I2C1 -> CR1 |= CR1_START;

	// wait until start flag is set
	WAIT_SR1(SR1_SB, I2C_ERR_START);

	// transmit slave address + write
	I2C1 -> DR = saddr << 1;

	// Wait until addr flag is set
	WAIT_ADDR();

	// Clear addr flag
	temp = I2C1 -> SR2;
	(void) temp;

	// Wait until transmitter empty
	WAIT_SR1(SR1_TXE, I2C_ERR_TXE);

	// Send memory address
	I2C1 -> DR = maddr;

	// Wait until transmitter empty
	WAIT_SR1(SR1_TXE, I2C_ERR_TXE);

	// Generate restart
	I2C1 -> CR1 |= CR1_START;

	// Wait until start flag is set
	WAIT_SR1(SR1_SB, I2C_ERR_START);

	// Transmit slave address + read
	I2C1 -> DR = saddr << 1 | 1;

	// Wait until addr flag is set
	WAIT_ADDR();

	// Disable acknowledge
	I2C1 -> CR1 &= ~CR1_ACK;

	// Clear address
	temp = I2C1 -> SR2;
	(void) temp;

	// Generate stop after data received
	I2C1 -> CR1 |= CR1_STOP;

	// Wait until RXNE flag is set
	WAIT_SR1(SR1_RXNE, I2C_ERR_RXNE);

	*data++ = I2C1 -> DR;

	return I2C_OK;
}

int I2C1_burst_read(char saddr, char maddr, int n, char *data) {
	volatile int temp;

	// Wait until bus not busy
	WAIT_NOT_BUSY();

	// Generate start
	I2C1 -> CR1 |= CR1_START;

	// Wait until start flag is set
	WAIT_SR1(SR1_SB, I2C_ERR_START);

	// Transmit slave address + write
	I2C1 -> DR = saddr << 1;

	// Wait until addr flag is set
	WAIT_ADDR();

	// Clear addr flag
	temp = I2C1 -> SR2;
	(void) temp;

	// Wait until transmitter is empty
	WAIT_SR1(SR1_TXE, I2C_ERR_TXE);
	// Send memory address
	I2C1 -> DR = maddr;
	// Wait until transmitter is empty
	WAIT_SR1(SR1_TXE, I2C_ERR_TXE);

	// Generate restart
	I2C1 -> CR1 |= CR1_START;
	// Wait until start flag is set
	WAIT_SR1(SR1_SB, I2C_ERR_START);
	// Transmit slave address + read
	I2C1 -> DR = saddr << 1 | 1;
	// Wait until addr flag is set
	WAIT_ADDR();
	// Clear addr flag
	temp = I2C1 -> SR2;
	(void) temp;
	// Enable acknowledge
	I2C1 -> CR1 |= CR1_ACK;
	while(n > 0U) {
		// If one byte
		if(n == 1U) {
			// Disable acknowledge
			I2C1 -> CR1 &= ~CR1_ACK;
			// Generate stop
			I2C1 -> CR1 |= CR1_STOP;
			// Wait for RXNE flag set
			WAIT_SR1(SR1_RXNE, I2C_ERR_RXNE);
			// Read data from DR
			*data++ = I2C1 -> DR;
			break;
		} else {
			// Wait until RXNE flag is set
			WAIT_SR1(SR1_RXNE, I2C_ERR_RXNE);
			// Read data from DR
			(*data++) = I2C1 -> DR;
			n--;
		}
	}

	return I2C_OK;
}

int I2C1_burst_write(char saddr, char maddr, int n, char *data) {
	volatile int temp;
	// Wait until bus is not busy
	WAIT_NOT_BUSY();
	// Generate start
	I2C1 -> CR1 |= CR1_START;
	// Wait until start flag is set
	WAIT_SR1(SR1_SB, I2C_ERR_START);
	// Transmit slave address
	I2C1 -> DR = saddr << 1;
	// Wait until addr flag is set
	WAIT_ADDR();
	// Clear addr flag
	temp = I2C1 -> SR2;
	(void) temp;
	// Wait until data register is empty
	WAIT_SR1(SR1_TXE, I2C_ERR_TXE);
	// Send memory address
	I2C1 -> DR = maddr;

	for(int i = 0; i < n; i++) {
		// Wait until data register is empty
		WAIT_SR1(SR1_TXE, I2C_ERR_TXE);
		// Transmit data byte
		I2C1 -> DR = *data++;
	}

	// Wait until transfer finished
	WAIT_SR1(SR1_BTF, I2C_ERR_BTF);
	// Generate stop
	I2C1 -> CR1 |= CR1_STOP;

	return I2C_OK;
}
