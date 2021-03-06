/*
 * Support for board peripherals that will control the Engineering Mode
 * Victor Manuel Fernandez Castro
 * 18 August 2015
 */

#include <stdlib.h>
#include <wiringPi.h>
#include <lcd.h>
#include <time.h>
#include <syslog.h>
#include <pthread.h>
#include <semaphore.h>
#include "player.h"
#include "values.h"
#include "output.h"

typedef enum button_t { CW, CCW, PUSH } button_t;
typedef enum periph_state_t { ENGINEER_OFF, MENU_ENGINEER, MENU_METRONOME, MENU_POWER, ENGINEER_ON, OPT_REBOOT, OPT_SHUTDOWN, OPT_BACK } periph_state_t;

// Rotary change on channel A
static void rot_change();

// Rotary push event
static void rot_push();

// Thread start point
static void* periph_run(void *arg);

static int lcd;											// LCD file descriptor
static int buttons[3] = { 0 };							// Array of pushed buttons
static pthread_t thread;								// Dispatching thread
static sem_t semaphore;									// Semaphore for synchronization
static struct timespec PUSH_DEBOUNCE = DEBOUNCE_TIME;	// Delay to debounce

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

// Destroys data and clears LCD

void periph_destroy() {
	lcdPosition(lcd, 0, 0);
	lcdClear(lcd);
}

// Rotary change on channel A

static void rot_change() {	
	int a = digitalRead(ROT_CH_A);
	int b = digitalRead(ROT_CH_B);
	
	// a==b => CW
	
	buttons[a != b] = 1;
	sem_post(&semaphore);
}

// Rotary push event

void rot_push() {
	nanosleep(&PUSH_DEBOUNCE, NULL);
	
	if (digitalRead(ROT_PUSH)) {
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
				state = MENU_ENGINEER;
				break;

			case MENU_ENGINEER:
				if (player_state(NULL) == ENGINEER)
					player_engineer_exit();

				state = MENU_METRONOME;
				break;
				
			case MENU_METRONOME:
				state = MENU_POWER;
				break;
				
			case MENU_POWER:
				state = ENGINEER_OFF;
				break;

			case ENGINEER_ON:
				output_noteoff(track, note + OUTPUT_OFFSET);
				note = (note + 1) % OUTPUT_LENGTH;
				output_noteon(track, note + OUTPUT_OFFSET);
				output_update();
				break;
				
			case OPT_REBOOT:
				state = OPT_SHUTDOWN;
				break;
			
			case OPT_SHUTDOWN:
				state = OPT_BACK;
				break;
				
			case OPT_BACK:
				state = OPT_REBOOT;
			}

		// Counterclockwise event

		} else if (buttons[CCW]) {
			buttons[CCW] = 0;

			switch (state) {
			case ENGINEER_OFF:
				state = MENU_POWER;
				break;

			case MENU_ENGINEER:
				if (player_state(NULL) == ENGINEER)
					player_engineer_exit();

				state = ENGINEER_OFF;
				break;
				
			case MENU_METRONOME:
				state = MENU_ENGINEER;
				break;
				
			case MENU_POWER:
				state = MENU_METRONOME;
				break;

			case ENGINEER_ON:
				output_noteoff(track, note + OUTPUT_OFFSET);
				note = (note + OUTPUT_LENGTH - 1) % OUTPUT_LENGTH;
				output_noteon(track, note + OUTPUT_OFFSET);
				output_update();
				break;
				
			case OPT_REBOOT:
				state = OPT_BACK;
				break;
				
			case OPT_SHUTDOWN:
				state = OPT_REBOOT;
				break;
				
			case OPT_BACK:
				state = OPT_SHUTDOWN;
			}

		// Push event

		} else if (buttons[PUSH]) {
			buttons[PUSH] = 0;

			switch (state) {
			case ENGINEER_OFF:
				break;

			case MENU_ENGINEER:
				if (player_state(NULL) != ENGINEER)
					player_engineer_enter();

				track = note = 0;
				output_noteon(track, note + OUTPUT_OFFSET);
				output_update();
				state = ENGINEER_ON;
				break;
				
			case MENU_METRONOME:
				output_metronome_enable(1 - output_metronome_enabled());
				break;
				
			case MENU_POWER:
				state = OPT_REBOOT;
				break;

			case ENGINEER_ON:
				output_noteoff(track, note + OUTPUT_OFFSET);

				if (++track == OUTPUT_NTRACKS)
					state = MENU_ENGINEER;
				else {
					note = 0;
					output_noteon(track, note + OUTPUT_OFFSET);
				}
				output_update();
				break;
				
			case OPT_REBOOT:
				if (system(CMD_REBOOT) == 0) {
					lcdClear(lcd);
					lcdPosition(lcd, 0, 1);
					lcdPuts(lcd, "   REINICIANDO...");
					return NULL;
				} else
					syslog(LOG_ERR, "Error rebooting");
				
				break;
				
			case OPT_SHUTDOWN:
				if (system(CMD_SHUTDOWN) == 0) {
					lcdClear(lcd);
					lcdPosition(lcd, 0, 1);
					lcdPuts(lcd, "    APAGANDO...");
					return NULL;
				} else
					syslog(LOG_ERR, "Error shutting down");
				
				break;
				
			case OPT_BACK:
				state = MENU_POWER;
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

		case MENU_ENGINEER:
			lcdPosition(lcd, 0, 0);
			lcdPuts(lcd, "   MODO INGENIERO");
			lcdPosition(lcd, 0, 2);
			
			if (player_state(NULL) == ENGINEER)
				lcdPuts(lcd, "      Activado");
			else
				lcdPuts(lcd, "    Desactivado");
			
			break;
			
		case MENU_METRONOME:
			lcdPosition(lcd, 0, 0);
			lcdPuts(lcd, "     METRONOMO");
			lcdPosition(lcd, 0, 2);
			
			if (output_metronome_enabled())
				lcdPuts(lcd, "      Activado");
			else
				lcdPuts(lcd, "    Desactivado");
			
			break;
			
		case MENU_POWER:
			lcdPosition(lcd, 0, 0);
			lcdPuts(lcd, " APAGAR / REINICIAR");
			break;

		case ENGINEER_ON:
			lcdPosition(lcd, 3, 0);
			lcdPuts(lcd, "MODO INGENIERO");
			lcdPosition(lcd, 0, 2);
			lcdPrintf(lcd, "Pista: %d", track);
			lcdPosition(lcd, 0, 3);
			lcdPrintf(lcd, "Nota:  %d", note);
			break;
			
		case OPT_REBOOT:
			lcdPosition(lcd, 0, 0);
			lcdPuts(lcd, " APAGAR / REINICIAR");
			lcdPosition(lcd, 0, 2);
			lcdPuts(lcd, "     Reiniciar");
			break;
			
		case OPT_SHUTDOWN:
			lcdPosition(lcd, 0, 0);
			lcdPuts(lcd, " APAGAR / REINICIAR");
			lcdPosition(lcd, 0, 2);
			lcdPuts(lcd, "       Apagar");
			break;
			
		case OPT_BACK:
			lcdPosition(lcd, 0, 0);
			lcdPuts(lcd, " APAGAR / REINICIAR");
			lcdPosition(lcd, 0, 2);
			lcdPuts(lcd, "       Atras");
		}

		// Wait for semaphore with timeout

		clock_gettime(CLOCK_REALTIME, &timeout);
		timeout.tv_sec += (int)LCD_TIMEOUT;
		timeout.tv_nsec += (int)((LCD_TIMEOUT - (int)LCD_TIMEOUT) * 1000000000);
		sem_timedwait(&semaphore, &timeout);
	}
	
	return NULL;
}
