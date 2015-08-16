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

#define BAUD_RATE B9600
#define UART_BUFFER_MAX 10
#define UART_BUFFER_MIN 10
#define FILE_BUFFER_MAX 4096

static const char UART_PATH[] = "/dev/ttyAMA0";
static const char CONFIG_PATH[] = "/etc/organ/remote.conf";

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
	tty = open(UART_PATH, O_RDONLY | O_NOCTTY);
	struct termios termios;

	if (tty < 0) {
		syslog(LOG_ERR, "open(): %m");
		return -1;
	}

	tcgetattr(tty, &oldtio);
	bzero(&termios, sizeof(termios));
	termios.c_cflag = CS8;
	termios.c_cc[VMIN] = UART_BUFFER_MIN;
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
	char buffer[UART_BUFFER_MAX + 1] = { 0 };

	while (1) {
		syslog(LOG_DEBUG, "Waiting for read...");
		int n = read(tty, buffer, UART_BUFFER_MAX);
		syslog(LOG_DEBUG, "Read %d: %s", n, buffer);
		
		if (n < 0) {
			syslog(LOG_ERR, "read(): %m");
			continue;
		}
		
		switch (buffer[7]) {
		case 'a':
			syslog(LOG_WARNING, "Battery low");
			
		case 'A':
			if (uart_playlist(0) < 0)
				syslog(LOG_ERR, "Error on uart_playlist()");
			
			break;
			
		case 'b':
			syslog(LOG_WARNING, "Battery low");
			
		case 'B':
			if (uart_playlist(1) < 0)
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
	int i, n;
	FILE *file = fopen(CONFIG_PATH, "r");
	char *next;
	char **playlist;
	char buffer[FILE_BUFFER_MAX];

	if (!file) {
		syslog(LOG_ERR, "fopen(): %m");
		return -1;
	}
	
	for (i = 0; i <= list; i++) {
		if (!fgets(buffer, FILE_BUFFER_MAX, file)) {
			syslog(LOG_ERR, "End of file reached");
			fclose(file);
			return -1;
		}
	}
	
	fclose(file);
	next = strtok(buffer, " \n");
	
	if (!next) {
		syslog(LOG_WARNING, "Playlist empty");
		return -1;
	}
	
	for (n = 1; (next = strtok(NULL, " \n")); n++);
	playlist = (char **)malloc(sizeof(char *) * n);
	next = buffer;
	
	for (i = 0; i < n; i++) {
		int length = strlen(next) + 1;
		playlist[i] = malloc(length);
		strcpy(playlist[i], next);
		next += length;
	}
	
	return player_start(playlist, n, 1);
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
