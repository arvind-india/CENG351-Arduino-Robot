#ifndef SFRONT_TRIG
#define SFRONT_TRIG 1
#endif

#ifndef SFRONT_ECHO
#define SFRONT_ECHO 2
#endif

#ifndef SSIDE_TRIG
#define SSIDE_TRIG 3
#endif

#ifndef SSIDE_ECHO
#define SSIDE_ECHO 4
#endif

void sonar_setup() {
  pinMode(SFRONT_TRIG, OUTPUT);
  pinMode(SFRONT_ECHO, INPUT);
  pinMode(SSIDE_TRIG, OUTPUT);
  pinMode(SSIDE_ECHO, INPUT);
}

double distance_cm(int trigpin, int echopin) {
  long duration;
  digitalWrite(trigpin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigpin, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(trigpin, LOW);
  duration = pulseIn(echopin, HIGH, 9001);
  return (duration / 58.2);
}

int front_distance() {
  return round(distance_cm(SFRONT_TRIG, SFRONT_ECHO));
}

int side_distance() {
  return round(distance_cm(SSIDE_TRIG, SSIDE_ECHO));
}

void sonar_selftest() {
  /* TEST SONAR */
  Serial.print("FRONT: ");
  Serial.print(front_distance());
  Serial.print("   | SIDE: ");
  Serial.print(side_distance());
  Serial.print('\n');
  delay(1000);
}

