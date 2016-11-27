#include "Arduino.h"

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

#define SFRONT_TRIG 13
#define SFRONT_ECHO 2
#define SSIDE_TRIG 4
#define SSIDE_ECHO 3


// Custom-written "libraries" for interacting with the robot.
// Call motor_speed() with a motor side and a speed (-255, 255)
// where -255 is full throttle backwards, 255 is full forwards,
// and 0 is stop.
#include "motors.h"
#include "linefollow.h"
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
  //sonar_selftest();
}

void follow_wall() {
  const int MAX_SPEED = 80;
  const int INCREMENT = 3;
  int left_speed = MAX_SPEED, right_speed = MAX_SPEED;
  const size_t avg_size = 2;
  double front_dist[avg_size], side_dist[avg_size];
  double avg_front_dist, avg_side_dist;
  size_t i = 0;
  boolean going = true;

  // initialize the rolling averages
  for (double dist = front_distance(); i < avg_size; i++)
    front_dist[i] = dist;
  avg_front_dist = front_dist[0];
  i = 0;
  for (double dist = side_distance(); i < avg_size; i++)
    side_dist[i] = dist;
  avg_side_dist = side_dist[0];
  
  while (going) {
    /* generate the rolling average distances */
    front_dist[i] = front_distance();
    side_dist[i] = side_distance();
    i = (i+1) % avg_size;
    for (size_t i = 0; i < avg_size; i++)
      avg_front_dist=(i==0?front_dist[i]:avg_front_dist+front_dist[i]);
    avg_front_dist /= (double) avg_size;  
    for (size_t i = 0; i < avg_size; i++)
      avg_side_dist=(i==0?side_dist[i]:avg_side_dist+side_dist[i]);
    avg_side_dist /= (double) avg_size;

    /* detect wall in front */
    if (avg_front_dist < 15 && avg_front_dist > 0) {
      /* spin 90 degrees right */
      motor_speed(LEFT_MOTOR, MAX_SPEED * 3 / 5 );
      motor_speed(RIGHT_MOTOR, -1 * MAX_SPEED );
      Serial.print("ROTATING             ");
      delay(300);
      left_speed = MAX_SPEED; right_speed = MAX_SPEED;

    /* detect if the wall is too far away */
    } else if (avg_side_dist > 10 || 
               avg_side_dist < 1) {
      right_speed = MAX_SPEED;
      // but don't let the left motor slow down too much
      if (left_speed > (MAX_SPEED * 2 / 5)) left_speed -= INCREMENT;
      Serial.print("SIDE IS OUT OF RANGE ");
      
    /* detect if the wall is too close */
    } else if (avg_side_dist < 8) {
      left_speed = MAX_SPEED;
      // but don't let the other motor slow down too much
      if (right_speed > (MAX_SPEED * 2 / 5)) right_speed -= INCREMENT;
      Serial.print("SIDE TOO CLOSE       ");
      
    /* detect that we are in the ideal range */
    } else {
      Serial.print("SIDE IN RANGE        ");
      left_speed = MAX_SPEED; right_speed = MAX_SPEED;
    }

    Serial.print("(side_dist = ");
    Serial.print(avg_side_dist);
    Serial.print("cm, front_dist = ");
    Serial.print(avg_front_dist);
    Serial.print("cm)\n");
    motor_speed(LEFT_MOTOR, left_speed);
    motor_speed(RIGHT_MOTOR, right_speed);
    delay(100);
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

  //Case L (we will always turn left after completing the 2nd stage, this way we can ensure we know exactly which direction we are facing)
  //The bot should turn left 90 degrees here and continue with follow_line()
  //Once the frontSensor == close stop following the line
  //Use the reedSwitch to check if the block is the magnet
  
  //If it is the magnet
      //the bot should pick it up
      //turn 180 degrees and drive until the rightLineSensor == black
  //If it is NOT the magnet
      //the bot should turn 180 degrees
      //continue follow_line()
      //drive until frontSensor == close
   
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
