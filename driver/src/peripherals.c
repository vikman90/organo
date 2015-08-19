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

#define ROT_CW 18	// Clockwise
#define ROT_CCW 24	// Counterclockwise
#define ROT_PUSH 23	// Button

#define TIMEOUT 1.0			// Waiting time to force LCD updating
#define BUFFER_LENGTH 256	// Buffer to receive file path from player

typedef enum button_t { CW, CCW, PUSH } button_t;
typedef enum state_t { ENGINEER_OFF, MENU, ENGINEER_ON };

// Rotary clockwise event
static void rot_cw();

// Rotary counterclockwise event
static void rot_ccw();

// Rotary push event
static void rot_push();

// Thread start point
static void* periph_run(void __attribute__((unused)) *arg);

static int lcd;							// LCD file descriptor
static int buttons[3] = { 0 };			// Array of pushed buttons
static state_t state = ENGINEER_OFF;	// Current state
static int track;						// Active track
static int note;						// Active note
static pthread_t thread;				// Dispatching thread
static sem_t semaphore;					// Semaphore for synchronization

// Initialize peripherals and start threads

int periph_init() {

	if (wiringPiSetupGpio()) {
		syslog(LOG_ERR, "Error at wiringPiSetupGpio()");
		return -1;
	}

	if (sem_init(&semaphore, 0, 0) {
		syslog(LOG_ERR, "sem_init(): %m");
		return -1;
	}

	lcd = lcdInit(LCD_ROWS, LCD_COLS, 4, LCD_RS, LCD_ES, LCD_D4, LCD_D5, LCD_D6, LCD_D7, 0, 0, 0, 0);

	if (lcd < 1) {
		syslog(LOG_ERR, "lcdInit(): %m");
		return -1;
	}

	if (pthread_create(&thread, NULL, periph_run, NULL) < 0) {
		syslog(LOG_ERR, "pthread_create(): %m");
		return -1;
	}

	wiringPiISR(ROT_CW, INT_EDGE_RISING, rot_cw);
	wiringPiISR(ROT_CCW, INT_EDGE_RISING, rot_ccw);
	wiringPiISR(ROT_PUSH, INT_EDGE_RISING, rot_push);

	return 0;

	// We must not destroy the semaphore because ISR threads can't be removed at this time
}

// Rotary clockwise event

void rot_cw() {
	buttons[CW] = 1;
	sem_post(&semaphore);
}

// Rotary counterclockwise event

void rot_ccw() {
	buttons[CCW] = 1;
	sem_post(&semaphore);
}

// Rotary push event

void rot_push() {
	buttons[PUSH] = 1;
	sem_post(&semaphore);
}

// Thread start point

void* periph_run(void *arg) {
	struct timespec timeout = { 0, 0 };
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
				state = ENGINEER_ON;
				break;

			case ENGINEER_ON:
				output_noteoff(track, note);

				if (++note == OUTPUT_NTRACKS)
					state = MENU;
				else {
					note = 0;
					output_noteon(track, note);
				}
			}
		}

		// Update LCD

		switch (state) {
		case ENGINEER_OFF:
			player_state_t plstate = player_state(filename);
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
			lcdPuts("MODO INGENIERO");
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
}
