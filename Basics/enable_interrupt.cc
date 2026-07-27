/* Title: interrupt handling exercise
 * Author: Mahmoud Ali
 * Description: Testing interrupts on tiny85
 * Created: Jul 03, 2024
 * Edited: Jul 03, 2024
 *
 */

// Import device headers and init program functions
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define TOG_BIT(Reg,BitNo)          Reg^=(1<<BitNo)                                     // Macro to toggle bit
#define BUTTON_PIN                  PB2                                                 // Button connected to pin PB2 
#define INTERRUPT_LED_PIN           PB0                                                 // LED connected to pin PB0


int main() {

              // Configure external interrupt on INT0 pin (rising edge trigger)
              GIMSK |= (1 << INT0);                           // Enable INT0 interrupt
              MCUCR |= (1 << ISC01);                          // Set interrupt on rising edge
              DDRB &= ~(1 << BUTTON_PIN);                     // Set PB2 (button pin) as input
              PORTB |= (1 << BUTTON_PIN);                     // Enable pull-up resistor for button (optional)

              DDRB |= (1 << INTERRUPT_LED_PIN);               // Configure LED pin as output

                // Enable global interrupt instruction should be added in next line
                sei();

              while (1) {

                // Main loop can do other tasks (e.g., Toggle another led)
                     }
            }

// Interrupt Service Routine for external interrupt 0
ISR(INT0_vect) {

            TOG_BIT(PORTB,INTERRUPT_LED_PIN);                   // Toggle LED connected on PB0
}