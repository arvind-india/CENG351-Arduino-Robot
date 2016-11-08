#ifndef LEFT_WHISKER
#define LEFT_WHISKER 12
#endif

#ifndef RIGHT_WHISKER
#define RIGHT_WHISKER 13
#endif

void whiskers_setup() {
	pinMode(LEFT_WHISKER, INPUT);
	pinMode(RIGHT_WHISKER, INPUT);
}

bool hit_detect(int whisker) {
	return (digitalRead(whisker) == HIGH);
}