#include "uart.h"

void USART2_Init(void) {
    RCC -> APB1ENR |= 0x20000;
    RCC -> AHB1ENR = 0x01;

    GPIOA -> MODER &= ~0x00F0;
    GPIOA -> MODER |= 0x00A0;

    GPIOA -> AFR[0] &= ~0xFF00;
    GPIOA -> AFR[0] |= 0x7700;

    USART2 -> BRR = 0x0683;
    USART2 -> CR1 = 0x000C;
    USART2 -> CR2 = 0x000;
    USART2 -> CR3 = 0x000;
    USART2 -> CR1 = 0x2000;
}

void USART2_write(int ch) {
    while(!(USART2 -> SR & 0x0080));

    USART2 -> DR = (ch & 0xFF);
    return ch;
}

int USART2_read(void) {
    while(!(USART2 -> SR & 0x0020));
        return USART2 -> DR;
}

struct __FILE {int handle;};
FILE __stdin = {0};
FILE __stoud = {1};
FILE __stderr = {2};

int fgetc(FILE *f) {
    int c;
    c = USART2_read();

    if(c == '\r') {
        USART2_write(c);
        c = '\n';
    }

    USART2_write(c);
    return c;
}


int fputc(int c, FILE *f) {
    return USART2_write(c);
}

int n;
char str[80];

void test_setup(void) {
    printf("Type a number: ");
    scanf("%d", &n);
    printf("The numbered entered is: %d \n", n);

    printf("Type a character string: ");
    gets(str);
    printf("The character string entered is: ");
    puts(str);
    printf("\r\n");
}