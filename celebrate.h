#ifndef CELEBRATION_PIN
#define CELEBRATION_PIN 12
#define ONCE (200)
#endif

void celebrate_setup() {
  pinMode(CELEBRATION_PIN, OUTPUT);
  digitalWrite(CELEBRATION_PIN, LOW);
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

void on(){
  dititalWrite(LED, HIGH);
}
void off(){
  dititalWrite(LED, LOW);
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

