#ifndef REED_SW_PIN
#define REED_SW_PIN 11
#endif

void reed_setup() {
  pinMode(REED_SW_PIN, INPUT);
}

bool reed_switch() {
  return digitalRead(REED_SW_PIN) == LOW;
}

