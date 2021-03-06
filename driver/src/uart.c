/*
 * Remote Receiver from UART
 * Victor Manuel Fernandez Castro
 * 14 August 2015
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <strings.h>
#include <termios.h>
#include <pthread.h>
#include <unistd.h>
#include <syslog.h>
#include <fcntl.h>
#include <stdio.h>
#include "player.h"
#include "database.h"
#include "values.h"

static struct termios oldtio;
static int tty = -1;

// Thread entry point
static void* uart_run(void *arg);

// Play a list
static int uart_playlist(int list);

// Pause or return playback
static int uart_pause();

// Initialization of the serial port

int uart_init() {
	return 0;
}

// Destroy the connection

void uart_destroy() {
	if (tty >= 0) {
		tcsetattr(tty, TCSANOW, &oldtio);
		close(tty);
	}
}

// Dispatching loop

int uart_loop() {
	static pthread_t thread;
	return pthread_create(&thread, NULL, uart_run, NULL) != 0;
}

// Thread entry point

void* uart_run(void __attribute__((unused)) *arg) {
	char buffer[UART_BUFFER_LENGTH];
	struct termios termios;
	
	tty = open(UART_PATH, O_RDONLY | O_NOCTTY);
	
	if (tty < 0) {
		syslog(LOG_ERR, "open(): %m");
		return NULL;
	}

	tcgetattr(tty, &oldtio);
	bzero(&termios, sizeof(termios));
	termios.c_cflag = CS7 | CLOCAL | CREAD;
	termios.c_iflag = IGNPAR;
	termios.c_cc[VMIN] = UART_BUFFER_LENGTH;
	cfsetispeed(&termios, UART_BAUDRATE);
	tcflush(tty, TCIFLUSH);
	tcsetattr(tty, TCSANOW, &termios);

	while (1) {
		int n = read(tty, buffer, UART_BUFFER_LENGTH);

		if (n < UART_BUFFER_LENGTH) {
			syslog(LOG_ERR, "Received only %d bytes from UART", n);
			return NULL;
		}

		if (strncmp(&buffer[8], "\r\n", 2)) {
			syslog(LOG_ERR, "Receiving error.\n");
			tcflush(tty, TCIFLUSH);
			continue;
		} else if (strncmp(buffer, UART_SERIAL, 7)) {
			syslog(LOG_ERR, "Invalid remote serial number");
			continue;
		}

		switch (buffer[7]) {
		case 'a':
			syslog(LOG_WARNING, "Battery low");

		case 'A':
			if (uart_playlist(1) < 0)
				syslog(LOG_ERR, "Error on uart_playlist()");

			break;

		case 'b':
			syslog(LOG_WARNING, "Battery low");

		case 'B':
			if (uart_playlist(2) < 0)
				syslog(LOG_ERR, "Error on uart_playlist()");

			break;

		case 'c':
			syslog(LOG_WARNING, "Battery low");

		case 'C':
			if (uart_pause() < 0)
				syslog(LOG_ERR, "Error on uart_pause()");

			break;

		default:
			syslog(LOG_ERR, "Button not recognised");
		}
	}

	return NULL;
}

// Play a list

int uart_playlist(int list) {
	int n, retval = 0;
	char **playlist;

	if (db_init() < 0) {
		syslog(LOG_ERR, "Error at db_init()");
		return -1;
	}

	n = db_query(&playlist, list);

	switch (n) {
	case -1:
		syslog(LOG_ERR, "Error at db_query()");
		retval = -1;

	case 0:
		syslog(LOG_ERR, "Playlist empty");
		retval = -1;

	default:
		retval = player_start(playlist, n, 1);
	}

	db_destroy();
	return retval;
}

// Pause or return playback

int uart_pause() {
	player_state_t state = player_state(NULL);

	switch (state) {
	case PAUSED:
		return player_resume();
	case PLAYING:
		return player_pause();
	default:
		return -1;
	}
}
