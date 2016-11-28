#ifndef CELEBRATION_PIN
#define CELEBRATION_PIN 12
#endif

void celebrate_setup() {
  pinMode(CELEBRATION_PIN, OUTPUT);
  digitalWrite(CELEBRATION_PIN, LOW);
}

void celebrate() {
  digitalWrite(CELEBRATION_PIN, HIGH);
  delay(500);
  digitalWrite(CELEBRATION_PIN, LOW);
  // etc
}

