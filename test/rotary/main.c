#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <wiringPi.h>

#define PIN1 6
#define PIN2 11

void button1() {
	printf("Boton 1\n");
}

void button2() {
	printf("Boton 2\n");
}

void onsigterm() {
	exit(0);
}

int main() {
	wiringPiSetupGpio();
	
	/*pinMode(PIN1, INPUT); 
	pinMode(PIN2, INPUT);
	pullUpDnControl(PIN1, PUD_DOWN);
	pullUpDnControl(PIN2, PUD_DOWN);*/
	
	wiringPiISR(PIN1, INT_EDGE_RISING, button1);
	wiringPiISR(PIN2, INT_EDGE_RISING, button2);
	
	signal(SIGTERM, onsigterm);
	pthread_exit(NULL);
}
