#define MASK(x) ((unsigned char) (1 << x))

void setup() {
    DDRB |= MASK(5);
}

void loop() {
    PORTB |= MASK(5);
    delay(500);
    PORTB &= ~MASK(5);
    delay(500);
}

