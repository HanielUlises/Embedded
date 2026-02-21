#ifndef __UART_H
#define __UART_H

#include "stm32f4xx.h"

#include <stdio.h>

void USART2_Init(void);
void USART2_write(int ch);
int fputc(int c, FILE *f);
void test_setup(void);

#endif