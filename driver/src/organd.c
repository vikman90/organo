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
#include "database.h"
#include "player.h"
#include "output.h"

#define BUFFER_LENGTH 256	// Length of receiving buffer
#define BACKLOG 5			// Listening queue length

static const char PID_PATH[] = "/run/organd.pid";		// Path for pid file
static const char SOCKET_PATH[] = "/run/organd.sock";	// Path for socket file
static const char LOG_IDENT[] = "organd";				// Logging identity

static char buffer[BUFFER_LENGTH];

// Cleanup function, called automatically on exiting.

static void cleanup() {
	unlink(PID_PATH);
	unlink(SOCKET_PATH);
	database_destroy();
	player_stop();
	output_destroy();
	closelog();
}

// Action on SIGTERM

static void onsigterm() {
	exit(EXIT_SUCCESS);
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
		syslog(LOG_ERR, "daemon(): %m\n");
		return -1;
	}

	atexit(cleanup);
	signal(SIGTERM, onsigterm);
	
	// PID file

	fd = open(PID_PATH, O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);

	if (fd < 0) {
		syslog(LOG_ERR, "open(): %m\n");
		return -1;
	}

	dprintf(fd, "%d", getpid());
	close(fd);
	
	// Socket
	
	sock = socket(AF_UNIX, SOCK_STREAM, 0);

	if (sock < 0) {
		syslog(LOG_ERR, "socket(): %m\n");
		return -1;
	}

	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, SOCKET_PATH);

	if (bind(sock, (const struct sockaddr *)&addr, sizeof(addr)) < 0) {
		syslog(LOG_ERR, "bind(): %m\n");
		return -1;
	}
	
	chmod(SOCKET_PATH, 0666);

	if (listen(sock, BACKLOG)) {
		syslog(LOG_ERR, "listen(): %m\n");
		return -1;
	}
	
	// Database and output

	if (database_init() < 0) {
		syslog(LOG_ERR, "Error at database_init()\n");
		return -1;
	}

	if (output_init() < 0) {
		syslog(LOG_ERR, "Error at output_init()\n");
		return -1;
	}
	
	return sock;
}

// Plays a playlist starting at a score

int playlist(const char *buffer) {
	int idplaylist, idscore, n;
	score_t *scores;
	
	sscanf(buffer, "%d %d", &idplaylist, &idscore);
	n = database_query(&scores, idplaylist);

	if (n < 0) {
		syslog(LOG_ERR, "playlist(): Playlist empty.\n");
		return -1;
	}
	
	if (player_state(NULL, NULL) != STOPPED) {
		if (player_stop() < 0) {
			syslog(LOG_ERR, "playlist(): Couldn't stop before playing.\n");
			return -1;
		}
	}

	return player_start(scores, n, idplaylist, idscore, 1);
}

int main() {
	int peer;
	int sock = setup();

	if (sock < 0)
		return EXIT_FAILURE;

	while (1) {
		peer = accept(sock, NULL, 0);

		if (peer < 0) {
			syslog(LOG_ERR, "accept(): %m\n");
			return EXIT_FAILURE;
		}

		if (recv(peer, buffer, BUFFER_LENGTH, 0) < 1) {
			syslog(LOG_ERR, "recv(): %m\n");
			send(peer, "ERROR", 5, 0);
		} else if (!strncmp(buffer, "PLAY", 4)) {		
			if (playlist(buffer + 5) < 0) {
				syslog(LOG_ERR, "Error at playlist()\n");
				send(peer, "ERROR", 5, 0);
			} else 
				send(peer, "OK", 2, 0);
		} else if (!strncmp(buffer, "STOP", 4)) {	
			if (player_stop() < 0) {
				syslog(LOG_ERR, "Error at player_stop()\n");
				send(peer, "ERROR", 5, 0);
			} else
				send(peer, "OK", 2, 0);
		} else if (!strncmp(buffer, "PAUSE", 5)) {			
			if (player_pause() < 0) {
				syslog(LOG_ERR, "Error at player_pause()\n");
				send(peer, "ERROR", 5, 0);
			} else
				send(peer, "OK", 2, 0);
		} else if (!strncmp(buffer, "RESUME", 6)) {			
			if (player_resume() < 0) {
				syslog(LOG_ERR, "Error at player_resume()\n");
				send(peer, "ERROR", 5, 0);
			} else
				send(peer, "OK", 2, 0);
		} else if (!strncmp(buffer, "STATUS", 6)) {			
			int idplaylist, idscore;
			enum player_state_t state = player_state(&idplaylist, &idscore);
			
			if (state == PLAYING) {
				sprintf(buffer, "PLAYING %d %d", idplaylist, idscore);
				send(peer, buffer, strlen(buffer), 0);
			} else if (state == PAUSED) {
				sprintf(buffer, "PAUSED %d %d", idplaylist, idscore);
				send(peer, buffer, strlen(buffer), 0);
			} else if (state == STOPPED)
				send(peer, "STOPPED", 7, 0);
			else {
				syslog(LOG_ERR, "Error: unknown state.\n");
				send(peer, "ERROR", 5, 0);
			}
		} else {
			syslog(LOG_ERR, "Error: unrecognised remote command.\n");
			send(peer, "ERROR", 5, 0);
		}
		
		if (close(peer) < 0) {
			syslog(LOG_ERR, "close(): %m\n");
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}
