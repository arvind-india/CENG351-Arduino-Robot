#ifndef SFRONT_TRIG
#define SFRONT_TRIG 1
#endif

#ifndef SFRONT_ECHO
#define SFRONT_ECHO 2
#endif

#ifndef SSIDE_TRIG
#define SSIDE_TRIG 4
#endif

#ifndef SSIDE_ECHO
#define SSIDE_ECHO 3
#endif

void sonar_setup() {
  pinMode(SFRONT_TRIG, OUTPUT);
  pinMode(SFRONT_ECHO, INPUT);
  pinMode(SSIDE_TRIG, OUTPUT);
  pinMode(SSIDE_ECHO, INPUT);
}

double distance_cm(int trigpin, int echopin) {
  long unsigned duration;
  digitalWrite(trigpin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigpin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigpin, LOW);
  delayMicroseconds(100);
  duration = pulseIn(echopin, HIGH, 60000);
  if (duration > 0) /* give 60ms for a read */
    delayMicroseconds(60000 - duration);
  return (duration / 58.24);
}

double front_distance() {
  return distance_cm(SFRONT_TRIG, SFRONT_ECHO);
}

double side_distance() {
  return distance_cm(SSIDE_TRIG, SSIDE_ECHO);
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


