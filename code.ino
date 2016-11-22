/* Pin Assignments */
#define LEFT_MOTOR_3A4A 6
#define LEFT_MOTOR_3A 8
#define LEFT_MOTOR_4A 7
#define RIGHT_MOTOR_1A2A 5
#define RIGHT_MOTOR_1A 10
#define RIGHT_MOTOR_2A 9

#define LEFT_WHISKER 12
#define RIGHT_WHISKER 13

#define LINEFOLLOW_LEFT A2
#define LINEFOLLOW_CENTER A0
#define LINEFOLLOW_RIGHT A1

#define SFRONT_TRIG 1
#define SFRONT_ECHO 2
#define SSIDE_TRIG 3
#define SSIDE_ECHO 4


// Custom-written "libraries" for interacting with the robot.
// Call motor_speed() with a motor side and a speed (-255, 255)
// where -255 is full throttle backwards, 255 is full forwards,
// and 0 is stop.
#include "motors.h"
#include "linefollow.h"
#include "whiskers.h"
#include "sonar.h"

void setup() {
  motors_setup();
  sonar_setup();
  linefollower_setup();

  Serial.begin(9600);
}

void loop() {
  //motor_selftest();
  follow_wall();
}

void follow_wall() {
  int left_speed = 100, right_speed = 100;
  int front_dist, side_dist;
  boolean going = true;
  
  while (going) {
    front_dist = front_distance();
    side_dist = side_distance();
    if (front_dist < 10 && front_dist > 2) {
      /* spin 90 degrees right */
      motor_speed(LEFT_MOTOR, 80);
      motor_speed(RIGHT_MOTOR, -100);
      Serial.print("ROTATING (front_dist = ");
      Serial.print(front_dist);
      Serial.print(")\n");
      while (front_dist < 10 && front_dist > 2) {
         front_dist = front_distance();
         delay(500);
      }
      left_speed = 100; right_speed = 100;
    } else if (side_dist > 30 || side_dist < 5) {
      if (left_speed > 60) left_speed -= 5;
      Serial.print("SIDE IS OUT OF RANGE (side_dist = ");
      Serial.print(side_dist);
      Serial.print(")\n");
    } else if (side_dist < 12) {
      right_speed -= 5;
      Serial.print("SIDE TOO CLOSE (side_dist = ");
      Serial.print(side_dist);
      Serial.print(")\n");
    } else {
      Serial.print("SIDE IN RANGE (side_dist = ");
      Serial.print(side_dist);
      Serial.print(")\n");
      left_speed = 100; right_speed = 100;
    }
    motor_speed(LEFT_MOTOR, left_speed);
    motor_speed(RIGHT_MOTOR, right_speed);
    delay(10);
  }
}

void follow_line(){
  bool on_track = true;

  Serial.println("following line!");
  
  // motor speed as a percent, -100 is full reverse
  left_speed = 100;
  right_speed = 100;
  
  while (on_track) {
    
    if (line_check(LEFT_LINESENSOR) == BLACK &&
        line_check(RIGHT_LINESENSOR) == BLACK) {
      Serial.println("Intersection.");
      left_speed = 0;
      right_speed = 0;
      on_track = false;
    } else if (line_check(LEFT_LINESENSOR) == BLACK) {
      Serial.println("steering left");
      left_speed -= 5;
    } else if (line_check(RIGHT_LINESENSOR) == BLACK) {
      Serial.println("steering right");
      right_speed -= 5;
    } else if (line_check(CENTER_LINESENSOR) == BLACK) {
      Serial.println("found middle");
      left_speed = 100;
      right_speed = 100;
    }

    motor_speed(LEFT_MOTOR, left_speed);
    motor_speed(RIGHT_MOTOR, right_speed);
    delay(50);
  }
}
