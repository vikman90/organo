/*
 * Test for LCD
 * Victor Manuel Fernandez Castro
 * 16 August 2015
 */

#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <lcd.h>

#define LCD_ROWS 4
#define LCD_COLS 20
#define PIN_RS 28	// BCM pin 20
#define PIN_ES 27	// BCM pin 16
#define PIN_D4 26	// BCM pin 12
#define PIN_D5 11	// BCM pin 7
#define PIN_D6 10	// BCM pin 8
#define PIN_D7 6	// BCM pin 25

int main() {
	int lcd = lcdInit(LCD_ROWS, LCD_COLS, 4, PIN_RS, PIN_ES, PIN_D4, PIN_D5, PIN_D6, PIN_D7, 0, 0, 0, 0);

	if (lcd < 0) {
		fprintf(stderr, "Error en lcdInit()\n");
		return EXIT_FAILURE;
	}

	//lcdClear(lcd);
	lcdPosition(lcd, 0, 0);
	lcdPuts(lcd, "Hola Mundo!");
	lcdPosition(lcd, 0, 1);
	lcdPuts(lcd, "Kaixo mundua!");
	lcdPosition(lcd, 0, 2);
	lcdPuts(lcd, "Hola món!");
	lcdPosition(lcd, 0, 3);
	lcdPuts(lcd, "Ola mundo!");

	getchar();
	return EXIT_SUCCESS;
}