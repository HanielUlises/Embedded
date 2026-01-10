#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define TOG_BIT(Reg,BitNo)          Reg^=(1<<BitNo) // Macro to toggle bit
#define BUTTON_PIN                  PB2             // Button connected to pin PB2 
#define INTERRUPT_BUZZER_PIN        PB0             // Buzzer connected to pin PB0
#define MAIN_PROG_LED_PIN           PB1             // LED connected to pin PB1

volatile unsigned char u8Counter = 0;

int main() {

            // Configure external interrupt on INT0 pin (rising edge trigger)
            GIMSK |= (1 << INT0);                           // Enable INT0 interrupt
            MCUCR |= (1 << ISC01);                          // Set interrupt on rising edge
            DDRB &= ~(1 << BUTTON_PIN);                     // Set PB2 (button pin) as input
            PORTB |= (1 << BUTTON_PIN);                     // Enable pull-up resistor for button (optional)


            DDRB |= (1 << INTERRUPT_BUZZER_PIN);            // Configure Buzzer pin as output

            sei();                                          // Enable global interrupts

            while (1) {
              // Main loop can do other tasks (e.g., Toggle another led)
              TOG_BIT(PORTB,MAIN_PROG_LED_PIN);                           // Toggle LED connected to PB1
            _delay_ms(1000);                               // Delay 1000ms between toggling
            }
          }

// Interrupt Service Routine for external interrupt 0
ISR(INT0_vect) {
          u8Counter++;                                                                             // counter to count number of times that interrupt triggered
          if(u8Counter == 3)                                                                 // checking if interrupt is triggered 3 times
          {
              TOG_BIT(PORTB,INTERRUPT_BUZZER_PIN);            // Toggle Buzzer state connected on PB0
              u8Counter = 0;                                                                    // Reset counter value to be 0
          }
}