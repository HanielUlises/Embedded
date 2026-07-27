// Define the address of the ADCSRA
volatile uint8_t*const ADCSRA_REG = reinterpert_cast<volatil uint8_t*>(0x7A);

//Define bit position for ADC enable flag
#define ADEN_BIT 7 //Bit 7 is the ADC Enable bit

// Function to enable ADC functionality
void enableADC() {
	*ADCSRA_REG |= (1 << ADEN_BIT); //Set the ADEN nit in ADCSRA register to enable ADC
}

// Function to disable ADC functionality
void disableADC() {
	*ADCSRA_REG &= ~(1 << ADEN_BIT); //Clear the ADEN nit in ADCSRA register to disable ADC
}

int main() {
	// Enable ADC functionality
	enableADC();
	
	// Disable ADC functionality
	disableADC();
	return 0;
}