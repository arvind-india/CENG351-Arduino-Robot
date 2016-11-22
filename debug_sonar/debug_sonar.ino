/*
 * Debugging routine for sonar sensors
 * 
 * The idea is to report any and all pulses received
 * by the echo pin of a sonar sensor
 * 
 * Pings will be triggered by writing to the Serial bus
 * 
 * Any and every ping that the echo recieves gets reported
 */


/* Pin Assignments */
typedef struct {
  byte trig;
  byte echo;
} sonar_sensor_t;
const sonar_sensor_t front = { 1, 2 },
                     side  = { 4, 3 };

void setup() {
  Serial.begin(9600);
  pinMode(front.trig, OUTPUT);
  pinMode(front.echo, INPUT);
  pinMode(side.trig, OUTPUT);
  pinMode(side.echo, INPUT);
}

void loop() { 
  static boolean ping_rec = false;
  static unsigned long pulsewidth;
  static unsigned long ping_sent_time;

  /* Check to see if something is recieved,
   * if so, then report it */
  if (ping_rec && pulsewidth > 0) {
    Serial.print("PING of width ");
    Serial.println(pulsewidth);
    Serial.print(" (distance = ");
    Serial.print(pulsewidth / 5824.0);
    Serial.print("m)\n");
    ping_rec = false;
  }

  /* Check to see if we said to send another ping */
  if (Serial.available()) {
    while (Serial.read() != -1);
    
    Serial.print("..... pinging ......\n");
    
    /* Send a sonar pulse, track when it was sent */
    digitalWrite(side.trig, LOW);
    delayMicroseconds(5);
    digitalWrite(side.trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(side.trig, LOW);
    delayMicroseconds(100); // time it takes for trigger to send
    pulsewidth = pulseIn(side.echo, HIGH, 60000);
    ping_rec = true;
  }
}
