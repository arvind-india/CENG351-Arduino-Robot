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
//  motor_selftest();
//  reed_selftest();
//  sonar_selftest();
  follow_wall();
//  follow_line();
}


void follow_wall() {
  const int MAX_SPEED = 70;
  const int MIN_TURN_SPEED = 40;
  const int INCREMENT = 5;
  int left_speed = MAX_SPEED, right_speed = MAX_SPEED;
  
  const size_t avg_size = 2;
  double front_dist[avg_size], side_dist[avg_size];
  double avg_front_dist, avg_side_dist;
  size_t i = 0;
  
  boolean going = true;
  unsigned long stuck_timer = 0;
  double last_front_dist, last_side_dist;

  unsigned long check_for_lines_timer = millis();
  unsigned short line_seen_counter = 0;

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
    if ((abs(avg_side_dist - last_side_dist) <= 1 || avg_side_dist > 100) &&
        (abs(avg_front_dist - last_front_dist) <= 1 || avg_front_dist > 100) &&
        avg_front_dist != 0 && avg_side_dist != 0) {

      /* 3 second "stuck timer" timeout */
      if ((millis() - stuck_timer) > (4*1000)) { 
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
    } else if (avg_side_dist > 8.75 || 
               avg_side_dist < 1.5) {
      right_speed = MAX_SPEED; //9 originally
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

    /* Countinually check to see if we happen to make it to 
       the line-sensor part of the course by accident, but only
       check if it's been long enough for that to happen 
       ( arbitarily picked number of seconds to wait before we should be
         clear of the start, but that we couldn't be through the tunnel yet
       
       ( this helps us ignore the black tape at the start) */
    if (millis() - check_for_lines_timer >= 5*1000 &&
       (line_check_raw(LEFT_LINESENSOR) <= 400 ||
        line_check_raw(RIGHT_LINESENSOR) <= 400 ||
        line_check_raw(CENTER_LINESENSOR) <= 400 )) {
      line_seen_counter++;
      if (line_seen_counter > 2)    
        going = false;
    }
  }
}



void follow_line(){
  const int MAX_SPEED = 90;
  const int INCREMENT = 5;
  const int MIN_TURN_SPEED = 50;

  const size_t avg_size = 3;
  double dists[avg_size];
  size_t dist_pos = 0;
  double avg_dist; 
  
  bool on_track = true;
  bool magnet = false;

  Serial.println("following line!");
  
  // motor speed as a percent, -100 is full reverse
  left_speed = MAX_SPEED;
  right_speed = MAX_SPEED;

  while (on_track) {
  
    if (line_check(LEFT_LINESENSOR) == BLACK &&
        line_check(RIGHT_LINESENSOR) == BLACK) {
      Serial.println("Intersection.");
      left_speed = 0;
      right_speed = 0;
      on_track = false;

    } else if (line_check(LEFT_LINESENSOR) == BLACK) {
      Serial.println("steering left");
      if (left_speed > MIN_TURN_SPEED) left_speed -= INCREMENT;
    } else if (line_check(RIGHT_LINESENSOR) == BLACK) {
      Serial.println("steering right");
      if (right_speed > MIN_TURN_SPEED) right_speed -= INCREMENT;
    } else if (line_check(CENTER_LINESENSOR) == BLACK) {
      Serial.println("found middle");
      left_speed = MAX_SPEED;
      right_speed = MAX_SPEED;
    }
    motor_speed(LEFT_MOTOR, left_speed);
    motor_speed(RIGHT_MOTOR, right_speed);
    delay(50);
  }

  /* Turn 90 degrees */
  motor_speed(LEFT_MOTOR, -1*MAX_SPEED);
  motor_speed(RIGHT_MOTOR, MAX_SPEED);
  delay(250);
  motor_speed(LEFT_MOTOR, 0);
  motor_speed(RIGHT_MOTOR, 0);
  delay(1000);
  

  /* Initialize rolling average for front ultrasonic sensor */
  for (size_t j = 0; j < avg_size; j++) {
    dists[j] = front_distance();
    avg_dist = ( j == 0 ? dists[j] : avg_dist + dists[j] );
  }
  avg_dist /= (double) avg_size;

  left_speed = MAX_SPEED;
  right_speed = MAX_SPEED;
  on_track = true;
  while (on_track) {
    dists[dist_pos] = front_distance();
    dist_pos = (dist_pos + 1 == avg_size ? 0 : dist_pos + 1);
    for (size_t j = 0; j < avg_size; j++) {
      avg_dist = ( j == 0 ? dists[j] : avg_dist + dists[j]);
    }
    avg_dist /= avg_size;
    
    if (avg_dist >= 1 && avg_dist <= 7) {
      Serial.println("Side found.");
      left_speed = 0;
      right_speed = 0;
      on_track = false;

    } else if (line_check(LEFT_LINESENSOR) == BLACK) {
      Serial.println("steering left");
      if (left_speed > MIN_TURN_SPEED) left_speed -= INCREMENT;
    } else if (line_check(RIGHT_LINESENSOR) == BLACK) {
      Serial.println("steering right");
      if (right_speed > MIN_TURN_SPEED) right_speed -= INCREMENT;
    } else if (line_check(CENTER_LINESENSOR) == BLACK) {
      Serial.println("found middle");
      left_speed = MAX_SPEED;
      right_speed = MAX_SPEED;
    }
    
    motor_speed(LEFT_MOTOR, left_speed);
    motor_speed(RIGHT_MOTOR, right_speed);
    //delay(50); /* Delay not needed b/c sonar call takes 60ms */
  }

  motor_speed(LEFT_MOTOR, 0);
  motor_speed(RIGHT_MOTOR, 0);
  digitalWrite(CELEBRATION_PIN, HIGH);
  delay(2000);
  digitalWrite(CELEBRATION_PIN, LOW);
  
  /* Once the robot is here, we've found the left side block. */
  /* so, check for the magnet */
  magnet = reed_switch();

  /* Spin a 180! */
  motor_speed(LEFT_MOTOR, -1*MAX_SPEED);
  motor_speed(RIGHT_MOTOR, MAX_SPEED);
  delay(500);
  motor_speed(LEFT_MOTOR, 0);
  motor_speed(RIGHT_MOTOR, 0);
  delay(1000);

 
  /* We have the magnet now, go until we find the left-turn "T"
     intersection to the garage and follow the line that way */
  if (magnet) {

  }

  /* The other side has the magnet, go over there (just like the
     last line-following chunk of code) and grab it,
     then 180, follow the track until seeing the right-turn "T"
     intersection to the garage, follow the line that way.*/
  else {

  }

  /* C E L E B R A T E */
  
}
