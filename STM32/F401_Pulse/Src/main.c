#include <stdint.h>

#define PERIPH_BASE           (0x40000000UL)
#define AHB1PERIPH_OFFSET     (0x00020000UL)
#define AHB1PERIPH_BASE       (PERIPH_BASE + AHB1PERIPH_OFFSET)

#define GPIOA_OFFSET          (0x0000U)
#define GPIOA_BASE            (AHB1PERIPH_BASE + GPIOA_OFFSET)

#define RCC_OFFSET            (0x3800UL)
#define RCC_BASE              (AHB1PERIPH_BASE + RCC_OFFSET)

#define AHB1EN_R_OFFSET       (0x30UL)
#define RCC_AHB1EN_R          (*(volatile unsigned int *)(RCC_BASE + AHB1EN_R_OFFSET))

#define MODE_R_OFFSET         (0x00UL)
#define GPIOA_MODE_R          (*(volatile unsigned int *)(GPIOA_BASE + MODE_R_OFFSET))

#define ODR_OFFSET            (0x14UL)
#define GPIOA_OD_R            (*(volatile unsigned int *)(GPIOA_BASE + ODR_OFFSET))

#define GPIOAEN               (1U << 0)
#define PIN5                  (1U << 5)
#define LED_PIN               PIN5

#define __IO volatile

//typedef struct {
//	 __IO uint32_t MODER;
//	 __IO uint32_t OTYPER;
//	 __IO uint32_t OSPEEDR;
//	 __IO uint32_t PUPDR;
//	 __IO uint32_t IDR;
//	 __IO uint32_t ODR;
//	 __IO uint32_t BSRR;
//	 __IO uint32_t LCKR;
//	 __IO uint32_t AFR[2];
//} GPIO_Typedef;

typedef struct {
	uint32_t DUMMY[12];
	__IO uint32_t AHB1ENR;
} RCC_TypeDef;

typedef struct {
	__IO uint32_t MODER;
	__IO uint32_t DUMMY[4];
	__IO uint32_t ODR;
} GPIO_TypeDef;

#define RCC 			((RCC_TypeDef*) RCC_BASE)
#define GPIOA 			((GPIO_TypeDef*) GPIOA_BASE)

int main(void)
{

    // Enable clock access to GPIOA
    // RCC_AHB1EN_R |= GPIOAEN;
	RCC -> AHB1ENR |= GPIOAEN;

    // Set PA5 as output pin (MODER5 = 01)
	// GPIOA_MODE_R |=  (1U << 10);   // set bit 10
	// GPIOA_MODE_R &= ~(1U << 11);   // clear bit 11
	GPIOA -> MODER |=  (1U << 10);   // set bit 10
	GPIOA -> MODER &= ~(1U << 11);   // clear bit 11

    while (1)
    {
        // Toggle PA5
    	GPIOA -> ODR ^= LED_PIN;
        for(int i =0; i < 100000; i++) {

        }
    }
}
