// Custom-written "library" for interacting with the robot.
// Call motor_speed() with a motor side and a speed (-255, 255)
// where -255 is full throttle backwards, 255 is full forwards,
// and 0 is stop.
#include "motor.h"

void setup() {
  /* call the "library" setup */
  motor_setup();
}

void loop() {
  int motors[2] = {LEFT_MOTOR, RIGHT_MOTOR};

  // for each motor, speed up, slow down,
  // go into reverse, go back to stop.
  for (int motor = 0; motor <= 1; motor++) { 
    for (int i = 0; i <= 255; i++) {
      motor_speed(motors[motor], i);
      delay(10);
    }
    for (int i = 255; i >= -255; i--) {
      motor_speed(motors[motor], i);
      delay(10);
    }
    for (int i = -255; i <= 0; i++) {
      motor_speed(motors[motor], i);
      delay(10);
    }
    delay(1000);
  }

  delay(2000);


}
