/*
 * Remote Receiver from UART
 * Victor Manuel Fernandez Castro
 * 14 August 2015
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <strings.h>
#include <termios.h>
#include <pthread.h>
#include <unistd.h>
#include <syslog.h>
#include <fcntl.h>
#include <stdio.h>
#include "player.h"
#include "database.h"

#define BAUD_RATE B9600
#define BUFFER_MAX 10
#define BUFFER_MIN 10

static const char UART_PATH[] = "/dev/ttyAMA0";

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
	struct termios termios;
	
	tty = open(UART_PATH, O_RDONLY | O_NOCTTY | O_NONBLOCK);
	
	if (tty < 0) {
		syslog(LOG_ERR, "open(): %m");
		return -1;
	}

	tcgetattr(tty, &oldtio);
	bzero(&termios, sizeof(termios));
	termios.c_cflag = CS8 | CLOCAL | CREAD;
	termios.c_cc[VMIN] = BUFFER_MIN;
	cfsetispeed(&termios, BAUD_RATE);
	tcsetattr(tty, TCSANOW, &termios);
	tcflush(tty, TCIFLUSH);

	return 0;
}

// Destroy the connection

void uart_destroy() {
	tcsetattr(tty, TCSANOW, &oldtio);
	close(tty);
}

// Dispatching loop

int uart_loop() {
	static pthread_t thread;
	return pthread_create(&thread, NULL, uart_run, NULL) != 0;
}

// Thread entry point

void* uart_run(void __attribute__((unused)) *arg) {
	char buffer[BUFFER_MAX + 1] = { 0 };

	while (1) {
		int n = read(tty, buffer, BUFFER_MAX);
		
		if (n < 0) {
			syslog(LOG_ERR, "read(): %m");
			break;
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
			
		case 'd':
			syslog(LOG_WARNING, "Battery low");
			
		case 'D':
			if (uart_pause() < 0)
				syslog(LOG_ERR, "Error on uart_pause()");
			
			break;
			
		case 'h':
			syslog(LOG_WARNING, "Battery low");
			
		case 'H':
			if (player_stop() < 0)
				syslog(LOG_ERR, "Error on player_stop()");
			
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
	
	if (db_init() < 1) {
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
