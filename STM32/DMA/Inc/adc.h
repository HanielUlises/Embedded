#ifndef ADC_H
#define ADC_H

#include <stdint.h>

void pa1_adc_interrupt_init(void);
void start_conversion(void);
uint32_t adc_read(void);

#define SR_EOC				(1U << 1)

#endif
