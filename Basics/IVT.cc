// Define interrupt vectors
#define RESET_VECTOR      0x0000
#define EXT_INT0_VECTOR   0x0004
#define TIMER0_VECTOR     0x0008
#define ADC_VECTOR        0x000C

// Declare ISRs
void ISR_Reset(void);
void ISR_EXT0(void);
void ISR_Timer0(void);
void ISR_ADC(void);

// Interrupt Vector Table (typically set up at a fixed memory location)
void (* const IVT[])(void) = {
      [RESET_VECTOR]      = ISR_Reset,
      [EXT_INT0_VECTOR]   = ISR_EXT0,
      [TIMER0_VECTOR]     = ISR_Timer0,
      [ADC_VECTOR]        = ISR_ADC,
};

// Example ISRs
void ISR_Reset(void) {
      // Reset routine
}

void ISR_EXT0(void) {
      // Handle external interrupt 0
}

void ISR_Timer0(void) {
      // Handle Timer 0 overflow
}

void ISR_ADC(void) {
      // Handle ADC conversion complete
}