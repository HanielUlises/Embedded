#define MEASURE

#define MASK(x) ((unsigned char) (1 << (x)))

#ifdef MEASURE
  uint32_t start_time, current_time;
#endif

void setup() {
  DDRB |= MASK(5);
  Serial.begin(9600);

}

void wait() {
  volatile uint32_t x = 100000;
  while(x) x--;
}

void loop() {
  PORTB ^= MASK(5);

  #ifdef MEASURE
    start_time = milis();
  #endif

  wait();
  wait();
  wait();
  wait();

  #ifdef MEASURE
    current_time = millis();
    Serial.print("Took");
    Serial.print((float) (current_time - start_time) * 0.001, 3);
    Serial.print(" seconds\n");
  #endif

}
