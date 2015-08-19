#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <wiringPi.h>

#define PIN_A 18
#define PIN_B 24
#define PIN_C 23

void changeA() {
	static int first = 1;
	int a, b;
	
	if (first) {
		first = 0;
		return;
	}
	
	a = digitalRead(PIN_A);
	b = digitalRead(PIN_B);
	
	printf("a = %d  b = %d\n", a, b);
}

void riseC() {
	static int first = 1;
	
	if (first)
		first = 0;
	else
		printf("Has pulsado.\n");
}

void onsigterm() {
	exit(0);
}

int main() {
	wiringPiSetupGpio();
	
	pinMode(PIN_A, INPUT);
	pinMode(PIN_B, INPUT);
	pinMode(PIN_C, INPUT);

	pullUpDnControl(PIN_A, PUD_UP);
	pullUpDnControl(PIN_B, PUD_UP);
	pullUpDnControl(PIN_C, PUD_DOWN);
	
	wiringPiISR(PIN_A, INT_EDGE_BOTH, changeA);
	wiringPiISR(PIN_C, INT_EDGE_RISING, riseC);
	
	signal(SIGTERM, onsigterm);
	pthread_exit(NULL);
}
