#ifndef CELEBRATION_PIN
#define CELEBRATION_PIN 12
#endif

#define ONCE (200)

/*
 * This is our robot celebration when it completes the course.  It 
 * celebrates by blinking yay in morse code.  
 */
void celebrate_setup() {
  pinMode(CELEBRATION_PIN, OUTPUT);
  digitalWrite(CELEBRATION_PIN, LOW);
}

void on(){
  digitalWrite(CELEBRATION_PIN, HIGH);
}
void off(){
  digitalWrite(CELEBRATION_PIN, LOW);
}

void dot(){
  on();
  delay(ONCE);
}

void dash(){
  on();
  delay(ONCE*3);
}

void next_thing(){
  off();
  delay(ONCE);
}

void next_letter(){
  off();
  delay(ONCE*3);
}

void next_word(){
  off();
  delay(ONCE*7);
}


void celebrate() {

  //Y
  dash();
  next_thing();
  dot();
  next_thing();
  dash();
  next_thing();
  dash();
  next_letter();

  //A
  dot();
  next_thing();
  dash();
  next_letter();

  //Y
  dash();
  next_thing();
  dot();
  next_thing();
  dash();
  next_thing();
  dash();
  next_word(); 
  
}
