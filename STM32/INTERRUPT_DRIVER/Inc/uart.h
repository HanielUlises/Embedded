#ifndef UART_H
#define UART_H

#include <stdint.h>
#include "stm32f4xx.h"

void dma1_stream6_init(uint32_t src, uint32_t dst, uint32_t len);
void uart2_rx_interrupt_init(void);
void uart2_tx_init(void);
void uart2_rxtx_interrupt_init(void);
char uart2_read(void);


#define SR_RXNE 			(1U << 5)

#endif
