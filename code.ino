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


// Custom-written "libraries" for interacting with the robot.
// Call motor_speed() with a motor side and a speed (-255, 255)
// where -255 is full throttle backwards, 255 is full forwards,
// and 0 is stop.
#include "motors.h"
#include "linefollow.h"
#include "whiskers.h"

void setup() {
  whiskers_setup();
  motors_setup();

  Serial.begin(9600);
}

void loop() {
//  motor_selftest();
  
/* hit detection test */
//  if (hit_detect(LEFT_WHISKER)) {
//    motor_speed(LEFT_MOTOR,-100);
//    delay(400);
//    motor_speed(LEFT_MOTOR,100);
//  }
//  if (hit_detect(RIGHT_WHISKER)) {
//    motor_speed(RIGHT_MOTOR,-100);
//    delay(400);
//    motor_speed(RIGHT_MOTOR,100);
//  }

/* debug code for the line followers */
//  size_t i;
//  i = Serial.print(line_check_raw(LEFT_LINESENSOR));
//  while (i < 8) { Serial.print(" "); i--;}
//  i = Serial.print(line_check_raw(CENTER_LINESENSOR));
//  while (i < 8) { Serial.print(" "); i--;}
//  Serial.print(line_check_raw(RIGHT_LINESENSOR));
//  Serial.print("\n");


/* Report the light level of each linesensor's state */
//  if (line_check(LEFT_LINESENSOR) == WHITE)
//    Serial.print("white  ");
//  else Serial.print("black  ");
//  if (line_check(CENTER_LINESENSOR) == WHITE)
//    Serial.print("white  ");
//  else Serial.print("black  ");
//  if (line_check(RIGHT_LINESENSOR) == WHITE)
//    Serial.print("white  ");
//  else Serial.print("black  ");
//
//  Serial.print("\n");
//  delay(500);


  follow_line();
  motor_speed(LEFT_MOTOR, -80);
  motor_speed(RIGHT_MOTOR, 80);
  delay(666);
  motor_speed(LEFT_MOTOR, 0);
  motor_speed(RIGHT_MOTOR, 0);
  delay(6000);
}


void follow_line()
{
  bool on_track = true;

  Serial.println("following line!");
  
  // motor speed as a percent, -100 is full reverse
  left_speed = 100;
  right_speed = 100;
  
  while (on_track)
  {
    if (hit_detect(LEFT_WHISKER) || hit_detect(RIGHT_WHISKER))
    {
      motor_speed(LEFT_MOTOR, -100);
      motor_speed(RIGHT_MOTOR, -100);
      Serial.println("WE HIT A WALL");
      delay(1000);
      on_track = false;
      motor_speed(LEFT_MOTOR, 0);
      motor_speed(RIGHT_MOTOR, 0);
      delay(5000);
    }
    
    if (line_check(LEFT_LINESENSOR) == BLACK &&
        line_check(RIGHT_LINESENSOR) == BLACK)
    {
      Serial.println("Intersection.");
      left_speed = 0;
      right_speed = 0;
      on_track = false;
    }
    else if (line_check(LEFT_LINESENSOR) == BLACK)
    {
      Serial.println("steering left");
      left_speed -= 5;
    }
    else if (line_check(RIGHT_LINESENSOR) == BLACK)
    {
      Serial.println("steering right");
      right_speed -= 5;
    }
    else if (line_check(CENTER_LINESENSOR) == BLACK)
    {
      Serial.println("found middle");
      left_speed = 100;
      right_speed = 100;
    }
    

    motor_speed(LEFT_MOTOR, left_speed);
    motor_speed(RIGHT_MOTOR, right_speed);
    delay(50);
  }
}
