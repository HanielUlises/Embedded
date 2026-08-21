#include <stdio.h>

#include "uart.h"
#include "adc.h"
#include "systick.h"
#include "tim.h"
#include "signals.h"

extern float _5hz_signal[HZ_5_SIG_LEN];


int main(void) {
	// Full access in CPACR to enable CP10 and CP11 (20-23)
	SCB -> CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2));

	SCB -> CPACR |= (1U << 20);
	SCB -> CPACR |= (1U << 21);
	SCB -> CPACR |= (1U << 22);
	SCB -> CPACR |= (1U << 23);

	while(1) {

	}
}
