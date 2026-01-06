#ifndef FASTGPIO_HPP
#define FASTGPIO_HPP

#include "main.h"  // For GPIO_TypeDef*, RCC, etc.

class FastGpio {
private:
    GPIO_TypeDef* port;
    uint16_t pin;
    uint32_t pinMask;

public:
    FastGpio(GPIO_TypeDef* gpio_port, uint16_t gpio_pin) 
        : port(gpio_port), pin(gpio_pin), pinMask(1u << gpio_pin) {
        
        // Enable clock (example: add cases for other ports)
        if (port == GPIOA) RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
        // Add for GPIOB, etc.

        // Configure as output
        port->MODER &= ~(3u << (pin * 2));
        port->MODER |= (1u << (pin * 2));
        port->OTYPER &= ~pinMask;
        port->OSPEEDR |= (3u << (pin * 2));  // High speed
        port->PUPDR &= ~(3u << (pin * 2));
    }

    void set()   { port->BSRR = pinMask; }
    void reset() { port->BSRR = (pinMask << 16); }
    void toggle(){ port->ODR ^= pinMask; }  // Alternative using ODR
    bool read()  { return (port->IDR & pinMask); }
};

#endif