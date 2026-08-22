#include <stdio.h>

#include "uart.h"
#include "adc.h"
#include "systick.h"
#include "tim.h"
#include "signals.h"
#include "arm_math.h"
#include "FreeRTOS.h"
#include "task.h"

extern float _5hz_signal[HZ_5_SIG_LEN];
extern float32_t input_signal_f32_1kHz_15kHz[KHZ1_15_SIG_LEN];
float g_input_sig_sample;

static void plot_input_signal(void);
static void pseudo_delay(int dly);
static void fpu_enable(void);
static void serial_plot_input(void);

int main(void) {
	fpu_enable();
	uart2_tx_init();

	while(1) {
//		plot_input_signal();	// SWV Data Trace: watch g_input_sig_sample
		serial_plot_input();	// UART: stream samples as ASCII for a serial plotter
	}
}

static void fpu_enable(void) {
	// Full access in CPACR to enable CP10 and CP11 (20-23)
	SCB -> CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2));
	// Enable floating point unit from CP10 and CP11 with full access
	SCB -> CPACR |= (1U << 20);
	SCB -> CPACR |= (1U << 21);
	SCB -> CPACR |= (1U << 22);
	SCB -> CPACR |= (1U << 23);

}

static void plot_input_signal(void) {
	int i;
//	for(i = 0; i < HZ_5_SIG_LEN; i++) {
//		g_input_sig_sample = _5hz_signal[i];
//	}

	for(i = 0; i < KHZ1_15_SIG_LEN; i++) {
		g_input_sig_sample = input_signal_f32_1kHz_15kHz[i];
		pseudo_delay(9000);
	}
}

static void serial_plot_input(void) {
	for (int i = 0; i < KHZ1_15_SIG_LEN; i++) {
		printf("%.6f\r\n", input_signal_f32_1kHz_15kHz[i]);
		pseudo_delay(9000);
	}
}

static void pseudo_delay(int dly) {
	for(int i = 0; i < dly; i++) {}
}
