#ifndef UART_H
#define UART_H

#include <stdint.h>
#include "stm32f4xx.h"

void uart2_rxtx_init(void);
char uart2_read(void);

#endif
