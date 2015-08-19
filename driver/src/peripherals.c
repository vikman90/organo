/*
 * Support for board peripherals that will control the Engineering Mode
 * Victor Manuel Fernandez Castro
 * 18 August 2015
 */

#include <wiringPi.h>
#include <lcd.h>
#include <time.h>
#include <syslog.h>
#include <pthread.h>
#include <semaphore.h>
#include "player.h"
#include "gpio.h"

#define LCD_ROWS 4
#define LCD_COLS 20
#define LCD_RS 20
#define LCD_ES 16
#define LCD_D4 12
#define LCD_D5 7
#define LCD_D6 8
#define LCD_D7 25

#define ROT_CH_A 18	// Clockwise
#define ROT_CH_B 24	// Counterclockwise
#define ROT_PUSH 23	// Button

#define TIMEOUT 1.0			// Waiting time to force LCD updating
#define BUFFER_LENGTH 256	// Buffer to receive file path from player

typedef enum button_t { CW, CCW, PUSH } button_t;
typedef enum periph_state_t { ENGINEER_OFF, MENU, ENGINEER_ON } periph_state_t;

// Rotary change on channel A
static void rot_change();

// Rotary push event
static void rot_push();

// Thread start point
static void* periph_run(void *arg);

static int lcd;								// LCD file descriptor
static int buttons[3] = { 0 };				// Array of pushed buttons
static pthread_t thread;					// Dispatching thread
static sem_t semaphore;						// Semaphore for synchronization

// Initialize peripherals and start threads

int periph_init() {

	if (wiringPiSetupGpio()) {
		syslog(LOG_ERR, "Error at wiringPiSetupGpio()");
		return -1;
	}

	if (sem_init(&semaphore, 0, 0)) {
		syslog(LOG_ERR, "sem_init(): %m");
		return -1;
	}

	lcd = lcdInit(LCD_ROWS, LCD_COLS, 4, LCD_RS, LCD_ES, LCD_D4, LCD_D5, LCD_D6, LCD_D7, 0, 0, 0, 0);

	if (lcd < 0) {
		syslog(LOG_ERR, "lcdInit(): %m");
		return -1;
	}

	if (pthread_create(&thread, NULL, periph_run, NULL) < 0) {
		syslog(LOG_ERR, "pthread_create(): %m");
		return -1;
	}
	
	pinMode(ROT_CH_A, INPUT);
	pinMode(ROT_CH_B, INPUT);
	pinMode(ROT_PUSH, INPUT);
	
	pullUpDnControl(ROT_CH_A, PUD_UP);
	pullUpDnControl(ROT_CH_B, PUD_UP);
	pullUpDnControl(ROT_PUSH, PUD_DOWN);

	wiringPiISR(ROT_CH_A, INT_EDGE_BOTH, rot_change);
	wiringPiISR(ROT_PUSH, INT_EDGE_RISING, rot_push);

	return 0;

	// We must not destroy the semaphore because ISR threads can't be removed at this time
}

// Rotary change on channel A

static void rot_change() {
	static int first = 1;
	int b;
	
	if (first) {
		first = 0;
		return;
	}
	
	b = digitalRead(ROT_CH_B);
	
	// b=1 => cw
	
	buttons[1 - b] = 1;
	sem_post(&semaphore);
}

// Rotary push event

void rot_push() {
	static int first = 1;
	
	if (first) {
		first = 0;
	} else {
		buttons[PUSH] = 1;
		sem_post(&semaphore);
	}
}

// Thread start point

void* periph_run(void __attribute__((unused)) *arg) {
	struct timespec timeout = { 0, 0 };
	periph_state_t state = ENGINEER_OFF;
	player_state_t plstate;
	int track = 0, note = 0;
	char filename[BUFFER_LENGTH];

	while (1) {

		// Clockwise event

		if (buttons[CW]) {
			buttons[CW] = 0;

			switch (state) {
			case ENGINEER_OFF:
				state = MENU;
				break;

			case MENU:
				if (player_state(NULL) == ENGINEER)
					player_engineer_exit();

				state = ENGINEER_OFF;
				break;

			case ENGINEER_ON:
				output_noteoff(track, note);
				note = (note + 1) % OUTPUT_LENGTH;
				output_noteon(track, note);
				output_update();
			}

		// Counterclockwise event

		} else if (buttons[CCW]) {
			buttons[CCW] = 0;

			switch (state) {
			case ENGINEER_OFF:
				state = MENU;
				break;

			case MENU:
				if (player_state(NULL) == ENGINEER)
					player_engineer_exit();

				state = ENGINEER_OFF;
				break;

			case ENGINEER_ON:
				output_noteoff(track, note);
				note = (note + OUTPUT_LENGTH - 1) % OUTPUT_LENGTH;
				output_noteon(track, note);
				output_update();
			}

		// Push event

		} else if (buttons[PUSH]) {
			buttons[PUSH] = 0;

			switch (state) {
			case ENGINEER_OFF:
				break;

			case MENU:
				if (player_state(NULL) != ENGINEER)
					player_engineer_enter();

				track = note = 0;
				output_noteon(track, note);
				output_update();
				state = ENGINEER_ON;
				break;

			case ENGINEER_ON:
				output_noteoff(track, note);

				if (++track == OUTPUT_NTRACKS)
					state = MENU;
				else {
					note = 0;
					output_noteon(track, note);
				}
				output_update();
			}
		}

		// Update LCD
		
		lcdClear(lcd);

		switch (state) {
		case ENGINEER_OFF:
			plstate = player_state(filename);
			lcdPosition(lcd, 0, 0);

			switch (plstate) {
			case PAUSED:
				lcdPuts(lcd, "      EN PAUSA");
				break;
			case PLAYING:
				lcdPuts(lcd, "   REPRODUCIENDO");
				lcdPosition(lcd, 0, 1);
				lcdPuts(lcd, filename);
				break;
			case STOPPED:
				lcdPuts(lcd, "      DETENIDO");
				break;
			default:
				lcdPuts(lcd, "      ERROR");
				syslog(LOG_ERR, "Inconsistent state on peripheral");
			}
			break;

		case MENU:
			lcdPosition(lcd, 3, 1);
			lcdPuts(lcd, "MODO INGENIERO");
			break;

		case ENGINEER_ON:
			lcdPosition(lcd, 3, 0);
			lcdPuts(lcd, "MODO INGENIERO");
			lcdPosition(lcd, 0, 2);
			lcdPrintf(lcd, "Pista: %d", track);
			lcdPosition(lcd, 0, 3);
			lcdPrintf(lcd, "Nota:  %d", note);
		}

		// Wait one second

		clock_gettime(CLOCK_REALTIME, &timeout);
		timeout.tv_sec += (int)TIMEOUT;
		timeout.tv_nsec += (int)((TIMEOUT - (int)TIMEOUT) * 1000000000);
		sem_timedwait(&semaphore, &timeout);
	}
	
	return NULL;
}
