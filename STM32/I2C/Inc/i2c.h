#ifndef I2C_H
#define I2C_H

/* Return codes: I2C_OK on success, negative on failure. Every error tells
 * you which stage of the transfer gave up, which is usually enough to
 * point at the wiring fault behind it. */
#define I2C_OK				(0)
#define I2C_ERR_BUSY		(-1)	/* bus stuck busy: SDA/SCL held low, missing pull-ups */
#define I2C_ERR_START		(-2)	/* start condition never generated */
#define I2C_ERR_ADDR		(-3)	/* addr phase timed out */
#define I2C_ERR_NACK		(-4)	/* slave did not ACK: wrong address or dead device */
#define I2C_ERR_TXE			(-5)	/* transmit register never emptied */
#define I2C_ERR_RXNE		(-6)	/* no data received */
#define I2C_ERR_BTF			(-7)	/* transfer never finished */

void I2C1_init(void);
int I2C1_byte_read(char saddr, char maddr, char *data);
int I2C1_burst_read(char saddr, char maddr, int n, char *data);
int I2C1_burst_write(char saddr, char maddr, int n, char *data);

#endif
