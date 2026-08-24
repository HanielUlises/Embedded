#ifndef TIM_H
#define TIM_H

void tim2_1hz_init(void);
void tim2_pa5_output_compare(void);
void tim3_pa6_input_capture(void);

#define SR_UIF 			(1U << 0)
#define SR_CC11F		(1U << 1)

#endif
