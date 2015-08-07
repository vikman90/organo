/*
 * Daemon for listening requests
 * Victor Manuel Fernandez Castro
 * 1 August 2015
 */

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <syslog.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <stdio.h>
#include "player.h"
#include "output.h"
#include "midi.h"

#define BUFFER_LENGTH 4096	// Length of receiving buffer
#define BACKLOG 5			// Listening queue length

static const char PID_PATH[] = "/run/organd.pid";		// Path for pid file
static const char SOCKET_PATH[] = "/run/organd.sock";	// Path for socket file
static const char LOG_IDENT[] = "organd";				// Logging identity

static char buffer[BUFFER_LENGTH];

// Cleanup function, called automatically on exiting.

static void cleanup() {
	player_stop();
	output_destroy();
	unlink(PID_PATH);
	unlink(SOCKET_PATH);
	closelog();
}

// Action on SIGTERM

static void onsigterm() {
	exit(EXIT_SUCCESS);
}

// Action on SIGPIPE

static void onsigpipe() {
	syslog(LOG_WARNING, "SIGPIPE received");
}

// Setup function. Returns socket id, or -1 on error.

static int setup() {
	int fd;
	int sock;
	struct sockaddr_un addr;
	
	// Logging
	
	openlog(LOG_IDENT, 0, LOG_DAEMON);
	
	// Daemon

	if (daemon(0, 0) < 0) {
		syslog(LOG_ERR, "daemon(): %m");
		return -1;
	}

	atexit(cleanup);
	signal(SIGTERM, onsigterm);
	signal(SIGPIPE, onsigpipe);
	
	// Clean files
	
	unlink(PID_PATH);
	unlink(SOCKET_PATH);
	
	// PID file

	fd = open(PID_PATH, O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);

	if (fd < 0) {
		syslog(LOG_ERR, "open(): %m");
		return -1;
	}

	dprintf(fd, "%d", getpid());
	close(fd);
	
	// Socket
	
	sock = socket(AF_UNIX, SOCK_STREAM, 0);

	if (sock < 0) {
		syslog(LOG_ERR, "socket(): %m");
		return -1;
	}

	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, SOCKET_PATH);

	if (bind(sock, (const struct sockaddr *)&addr, sizeof(addr)) < 0) {
		syslog(LOG_ERR, "bind(): %m");
		return -1;
	}
	
	chmod(SOCKET_PATH, 0666);

	if (listen(sock, BACKLOG)) {
		syslog(LOG_ERR, "listen(): %m");
		return -1;
	}
	
	// Output

	if (output_init() < 0) {
		syslog(LOG_ERR, "Error at output_init()");
		return -1;
	}
	
	return sock;
}

// Play a list of files

int play(char *arg, int loop) {
	char **playlist;
	int i, n = atoi(arg);
	
	if (n < 1) {
		syslog(LOG_ERR, "Playlist size < 1");
		return -1;
	}
	
	playlist = (char **)malloc(sizeof(char *) * n);
	arg = strtok(arg, " ");
	
	for (i = 0; i < n; i++) {
		arg = strtok(NULL, " ");
		
		if (arg) {
			playlist[i] = malloc(strlen(arg) + 1);
			strcpy(playlist[i], arg);
		} else
			break;
	}
	
	if (i < n) {
		syslog(LOG_ERR, "File list terminated prematurely.");
		
		while (i > 0)
			free(playlist[--i]);
		
		free(playlist);
		return -1;
	}
	
	return player_start(playlist, n, loop);
}

int main() {
	int peer, bytes;
	int sock = setup();

	if (sock < 0)
		return EXIT_FAILURE;

	while (1) {
		peer = accept(sock, NULL, 0);

		if (peer < 0) {
			syslog(LOG_ERR, "accept(): %m");
			return EXIT_FAILURE;
		}
		
		bytes = recv(peer, buffer, BUFFER_LENGTH, 0);

		if (bytes < 1) {
			syslog(LOG_ERR, "recv(): %m");
			send(peer, "ERROR", 5, 0);
		} else if (!strncmp(buffer, "PLAYLOOP", 8)) {
			buffer[bytes] = '\0';
			
			if (play(buffer + 9, 1) < 0) {
				syslog(LOG_ERR, "Error at play()");
				send(peer, "ERROR", 5, 0);
			} else 
				send(peer, "OK", 2, 0);
		} else if (!strncmp(buffer, "PLAY", 4)) {
			buffer[bytes] = '\0';
			
			if (play(buffer + 5, 0) < 0) {
				syslog(LOG_ERR, "Error at play()");
				send(peer, "ERROR", 5, 0);
			} else 
				send(peer, "OK", 2, 0);
		} else if (!strncmp(buffer, "STOP", 4)) {	
			if (player_stop() < 0) {
				syslog(LOG_ERR, "Error at player_stop()");
				send(peer, "ERROR", 5, 0);
			} else
				send(peer, "OK", 2, 0);
		} else if (!strncmp(buffer, "PAUSE", 5)) {	
			if (player_pause() < 0) {
				syslog(LOG_ERR, "Error at player_pause()");
				send(peer, "ERROR", 5, 0);
			} else
				send(peer, "OK", 2, 0);
		} else if (!strncmp(buffer, "RESUME", 6)) {			
			if (player_resume() < 0) {
				syslog(LOG_ERR, "Error at player_resume()");
				send(peer, "ERROR", 5, 0);
			} else
				send(peer, "OK", 2, 0);
		} else if (!strncmp(buffer, "STATUS", 6)) {			
			const char *path;
			enum player_state_t state = player_state(&path);
			
			switch (state) {
			case PAUSED:
				sprintf(buffer, "PAUSED %s", path);				
				send(peer, buffer, strlen(buffer), 0);
				break;
				
			case PLAYING:
				sprintf(buffer, "PLAYING %s", path);
				send(peer, buffer, strlen(buffer), 0);
				break;
				
			case STOPPED:
				send(peer, "STOPPED", 7, 0);
				break;
			default:
				syslog(LOG_ERR, "Error: unknown state.");
				send(peer, "ERROR", 5, 0);
			}
		} else {
			syslog(LOG_ERR, "Error: unrecognised remote command.");
			send(peer, "ERROR", 5, 0);
		}
		
		if (close(peer) < 0) {
			syslog(LOG_ERR, "close(): %m");
		}
	}

	return EXIT_SUCCESS;
}
