#include "Arduino.h"

/* Pin Assignments */
#define LEFT_MOTOR_3A4A 6
#define LEFT_MOTOR_3A 8
#define LEFT_MOTOR_4A 7
#define RIGHT_MOTOR_1A2A 5
#define RIGHT_MOTOR_1A 10
#define RIGHT_MOTOR_2A 9

#define LINEFOLLOW_LEFT A2
#define LINEFOLLOW_CENTER A0
#define LINEFOLLOW_RIGHT A1

#define SFRONT_TRIG 13
#define SFRONT_ECHO 2
#define SSIDE_TRIG 4
#define SSIDE_ECHO 3

#define REED_SW_PIN 11

#define CELEBRATION_PIN 12


// Custom-written "libraries" for interacting with the robot.
// Call motor_speed() with a motor side and a speed (-255, 255)
// where -255 is full throttle backwards, 255 is full forwards,
// and 0 is stop.
#include "motors.h"
#include "linefollow.h"
#include "sonar.h"
#include "reed.h"
#include "celebrate.h"


void setup() {
  motors_setup();
  sonar_setup();
  linefollower_setup();
  reed_setup();
  celebrate_setup();

  Serial.begin(9600);
}

void loop() {
  //motor_selftest();
  //reed_selftest();
  //sonar_selftest();
  //follow_wall();
  //follow_line();
  //stage_2();
  celebrate();
  delay(2000);
}


void follow_wall() {
  const int MAX_SPEED = 80;
  const int MIN_TURN_SPEED = 45;
  const int INCREMENT = 4;
  int left_speed = MAX_SPEED, right_speed = MAX_SPEED;
  
  const size_t avg_size = 2;
  double front_dist[avg_size], side_dist[avg_size];
  double avg_front_dist, avg_side_dist;
  size_t i = 0;
  
  boolean going = true;
  unsigned long stuck_timer = 0;
  double last_front_dist, last_side_dist;

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
    last_side_dist = avg_side_dist;
    last_front_dist = avg_front_dist;
    for (size_t j = 0; j < avg_size; j++)
      avg_front_dist=(j==0?front_dist[j]:avg_front_dist+front_dist[j]);
    avg_front_dist /= (double) avg_size;  
    for (size_t j = 0; j < avg_size; j++)
      avg_side_dist=(j==0?side_dist[j]:avg_side_dist+side_dist[j]);
    avg_side_dist /= (double) avg_size;

    /* detect when both distance sensor averages are staying */
    if ((abs(avg_side_dist - last_side_dist) <= 1) &&
        (abs(avg_front_dist - last_front_dist) <= 1) &&
        avg_front_dist != 0 && avg_side_dist != 0) {

      /* six second "stuck timer" timeout */
      if ((millis() - stuck_timer) > (3*1000)) { 
        motor_speed(LEFT_MOTOR, -70);
        motor_speed(RIGHT_MOTOR, -70);
        delay(500);
        stuck_timer = millis();
      }
    } else {
      stuck_timer = millis();
    }

    /* detect wall in front */
    if (avg_front_dist < 15 && avg_front_dist > 0) {
      /* spin 90 degrees right */
      motor_speed(LEFT_MOTOR, MAX_SPEED * 3 / 5 );
      motor_speed(RIGHT_MOTOR, -1 * MAX_SPEED );
      Serial.print("ROTATING             ");
      delay(300);
      left_speed = MAX_SPEED; right_speed = MAX_SPEED;

    /* detect if the wall is too far away */
    } else if (avg_side_dist > 9 || 
               avg_side_dist < 1) {
      right_speed = MAX_SPEED;
      // but don't let the left motor slow down too much
      if (left_speed > MIN_TURN_SPEED) left_speed -= INCREMENT;
      Serial.print("SIDE IS OUT OF RANGE ");
      
    /* detect if the wall is too close */
    } else if (avg_side_dist < 6) {
      left_speed = MAX_SPEED;
      // but don't let the other motor slow down too much
      if (right_speed > MIN_TURN_SPEED) right_speed -= INCREMENT;
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
    delay(50);
  }
}



void follow_line(){
  bool on_track = true;
  bool magnet = false; 
  int bot_dir = 0; 
    // bot_dir  odd = left, even = right

  Serial.println("following line!");
  
  // motor speed as a percent, -100 is full reverse
  left_speed = 80;
  right_speed = 80;
 
  while (!magnet) {     
  
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

    //when on_track == false we need to turn [LEFT] then continue line following
    //code to turn left below - check if it actually turns 90 deg
    motor_speed(LEFT_MOTOR, -100);
    motor_speed(RIGHT_MOTOR, 100);
    delay(250);
    motor_speed(LEFT_MOTOR, 0);
    motor_speed(RIGHT_MOTOR, 0);

    //TODO: REMOVE THIS, THIS IS ONCE WE'VE EXITED THE TUNNEL
    delay(25000);
    

//    while (true) {
//      double dist_from_wall = front_distance();
//      if (dist_from_wall > 0 && dist_from_wall < 6.5) {
//        break; 
//      } else if (line_check(LEFT_LINESENSOR) == BLACK) {
//        Serial.println("steering left");
//        left_speed -= 5;
//      } else if (line_check(RIGHT_LINESENSOR) == BLACK) {
//        Serial.println("steering right");
//        right_speed -= 5;
//      } else if (line_check(CENTER_LINESENSOR) == BLACK) {
//        Serial.println("found middle");
//        left_speed = 100;
//        right_speed = 100;
//      }
//      motor_speed(LEFT_MOTOR, left_speed);
//      motor_speed(RIGHT_MOTOR, right_speed);
//      delay(50);
//    }
// //check the reed_switch function, if it returns false it should turn 180 deg and continue line following
//    magnet = reed_switch();
//
//   
//    if (magnet == false) {
//      motor_speed (LEFT_MOTOR, -100);
//      motor_speed (RIGHT_MOTOR, 100);
//      delay (500);
//
//          while (true) {
//            double dist_from_wall = front_distance();
//            if (dist_from_wall > 0 && dist_from_wall < 6.5) {
//             break; 
//            } else if (line_check(LEFT_LINESENSOR) == BLACK) {
//              Serial.println("steering left");
//             left_speed -= 5;
//            } else if (line_check(RIGHT_LINESENSOR) == BLACK) {
//             Serial.println("steering right");
//              right_speed -= 5;
//           } else if (line_check(CENTER_LINESENSOR) == BLACK) {
//              Serial.println("found middle");
//              left_speed = 100;
//              right_speed = 100;
//            }
//            motor_speed(LEFT_MOTOR, left_speed);
//            motor_speed(RIGHT_MOTOR, right_speed);
//           delay(50);
//           }
//    }
  }
}
  


bool go_through(int distance){
  if (distance < 10){
     return false;
  }
  else if (distance > 11){
    return true;
  }
}

void stage_2() {
  /* 
   *  This section of the code should complete part two of the 
   *  obstacle course, given a boolean value saying that the robot 
   *  has entered that part of the course.  
   */
  bool enter = true; //Has the robot entered the second stage?  
  bool facing; //when true, facing left.  when false, facing right
  if (enter == true){
  
    motor_speed(LEFT_MOTOR, 80);
    motor_speed(RIGHT_MOTOR, -80);
    delay(500);
  
    while (go_through(side_distance()) != true || front_distance() > 3) {   
      motor_speed(LEFT_MOTOR, 80);
      motor_speed(RIGHT_MOTOR, 80);
    }
    if(go_through(side_distance()) == true){
      motor_speed(LEFT_MOTOR, 80);
      motor_speed(RIGHT_MOTOR, -80);
      delay(125);
      motor_speed(LEFT_MOTOR, 100);
      motor_speed(RIGHT_MOTOR, 100);
      delay(1000);
    }
    if (front_distance() > 3){
      motor_speed(LEFT_MOTOR, 80);
      motor_speed(RIGHT_MOTOR, -80);
      delay(500);
      while(front_distance() > 3){
        motor_speed(LEFT_MOTOR, 80);
        motor_speed(RIGHT_MOTOR, 80);
        facing = false;
      }
    }
  }
}

