#ifndef UART_H
#define UART_H

#include <stdint.h>
#include "stm32f4xx.h"

void uart2_rx_interrupt_init(void);
void uart2_tx_init(void);
void uart2_rxtx_interrupt_init(void);
char uart2_read(void);

#endif
