#define HUEAUEAUEUAEUAEUAE Serial.print("fucker\n");
#define WHARRGARBL Serial.begin(9600);
/* Arduino pin assignments, these should be made in the 
 * document which uses the library, but in case they aren'table
 * defined, we fall back to these defaults */
#ifndef LINEFOLLOW_LEFT
#define LINEFOLLOW_LEFT A3
#endif

#ifndef LINEFOLLOW_CENTER
#define LINEFOLLOW_CENTER A0
#endif

#ifndef LINEFOLLOW_RIGHT
#define LINEFOLLOW_RIGHT A1
#endif

typedef enum {
	LEFT_LINESENSOR,
	CENTER_LINESENSOR,
	RIGHT_LINESENSOR
} 
linesensor_t;

typedef enum {
	WHITE,
	BLACK
} 
lightlevel_t;

void linefollower_setup() {
	pinMode(LINEFOLLOW_LEFT, INPUT);
	pinMode(LINEFOLLOW_RIGHT, INPUT);
	pinMode(LINEFOLLOW_CENTER, INPUT);
}

int line_check_raw(linesensor_t sens) {
	int lightlevel = -1;
	switch (sens) {
		case LEFT_LINESENSOR:
			lightlevel = analogRead(LINEFOLLOW_LEFT);
			break;
		case CENTER_LINESENSOR:
			lightlevel = analogRead(LINEFOLLOW_CENTER);
			break;
		case RIGHT_LINESENSOR:
			lightlevel = analogRead(LINEFOLLOW_RIGHT);
			break;
	}
	return lightlevel;
}


lightlevel_t line_check(linesensor_t sens) {
	return line_check_raw(sens) < 800 ? WHITE : BLACK;
}
