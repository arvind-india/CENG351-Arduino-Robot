#ifndef REED_SW_PIN
#define REED_SW_PIN 11
#endif

void reed_setup() {
  pinMode(REED_SW_PIN, INPUT);
}

bool reed_switch() {
  return digitalRead(REED_SW_PIN) == LOW;
}

void reed_selftest() {
  Serial.print("reed switch = ");
  if (reed_switch())
    Serial.print("MAGNET!\n");
  else
    Serial.print(":(\n");
  delay(500);
}

