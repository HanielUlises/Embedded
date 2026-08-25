#ifndef UART_H
#define UART_H

#include <stdint.h>
#include "stm32f4xx.h"

void dma1_stream6_init(uint32_t src, uint32_t dst, uint32_t len);
void uart2_rx_interrupt_init(void);
void uart2_tx_init(void);
void uart2_rxtx_interrupt_init(void);
char uart2_read(void);

#define HISR_TCIF6			(1U << 21)
#define SR_RXNE 			(1U << 5)
#define HIFCR_CTCIF6		(1U << 21)

#endif
