/*
 * Prueba inicial: dsPIC33CK64MC105 Curiosity Nano
 *  - LED0 (RD10, activo en bajo) parpadea cada 500 ms
 *  - Envía un contador por UART1 -> CDC del depurador (/dev/ttyACM0, 9600 8N1)
 *  - SW0 (RD13, activo en bajo) imprime un mensaje al presionarse
 *
 * Reloj: FRC interno 8 MHz sin PLL -> Fcy = 4 MHz.
 */
#define FCY 4000000UL

#include <xc.h>
#include <libpic30.h>
#include <stdio.h>

#pragma config FNOSC = FRC
#pragma config IESO = OFF
#pragma config POSCMD = NONE
#pragma config FCKSM = CSDCMD
#pragma config FWDTEN = ON_SW
#pragma config ICS = PGD3
#pragma config JTAGEN = OFF

#define BAUD 9600UL

#define LED_ON()    (_LATD10 = 0)
#define LED_TOGGLE() (_LATD10 ^= 1)
#define SW0_PRESSED() (_RD13 == 0)

static void gpio_init(void)
{
    _TRISD10 = 0;
    _LATD10 = 1;        /* LED apagado */
    _TRISD13 = 1;
    _CNPUD13 = 1;       /* pull-up para SW0 */
}

static void uart1_init(void)
{
    _TRISC10 = 0;       /* U1TX -> CDC RX del depurador */
    _TRISC11 = 1;       /* U1RX <- CDC TX del depurador */
    _CNPUC11 = 1;       /* la línea flota hasta que se abre el puerto */

    _RP58R = 1;         /* RP58 (RC10) = U1TX */
    _U1RXR = 59;        /* U1RX = RP59 (RC11) */

    U1MODE = 0;
    U1MODEH = 0;        /* reloj de UART = Fosc/2 = Fcy */
    U1MODEbits.BRGH = 1;
    U1BRG = (uint16_t)(FCY / (4 * BAUD) - 1);
    U1MODEbits.UARTEN = 1;
    U1MODEbits.UTXEN = 1;
    U1MODEbits.URXEN = 1;
}

static void uart1_putc(char c)
{
    while (U1STAHbits.UTXBF);
    U1TXREG = c;
}

static void uart1_puts(const char *s)
{
    while (*s)
        uart1_putc(*s++);
}

int main(void)
{
    char line[48];
    uint16_t n = 0;
    uint8_t sw_prev = 0;

    gpio_init();
    uart1_init();

    uart1_puts("\r\nDAQ dsPIC33CK64MC105 - prueba inicial\r\n");

    while (1) {
        LED_TOGGLE();
        snprintf(line, sizeof line, "hola %u\r\n", n++);
        uart1_puts(line);

        for (uint8_t i = 0; i < 50; i++) {
            uint8_t sw = SW0_PRESSED();
            if (sw && !sw_prev)
                uart1_puts("SW0 presionado\r\n");
            sw_prev = sw;
            __delay_ms(10);
        }
    }
}
