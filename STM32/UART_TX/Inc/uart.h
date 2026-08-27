#ifndef UART_H
#define UART_H

#include <stdint.h>
#include "stm32f4xx.h"

#define GPIOAEN 		(1U << 0)
#define UART2EN			(1U << 17)


void uart2_rxtx_init(void);
char uart2_read(void);

#endif
