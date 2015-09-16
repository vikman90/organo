/*
 * Local socket management for the protocol of the organ control
 * Victor Manuel Fernandez Castro
 * 8 August 2015
 */

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <syslog.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "player.h"
#include "values.h"

static int sock = -1;
static char buffer[BUFFER_LENGTH];

static int play(char *arg, int loop);

// Initialization of the socket

int socket_init(int uid, int gid) {
	struct sockaddr_un addr;

	sock = socket(AF_UNIX, SOCK_STREAM, 0);

	if (sock < 0) {
		syslog(LOG_ERR, "socket(): %m");
		return -1;
	}
	
	unlink(SOCKET_PATH);
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, SOCKET_PATH);

	if (bind(sock, (const struct sockaddr *)&addr, sizeof(addr)) < 0) {
		syslog(LOG_ERR, "bind(): %m");
		return -1;
	}

	if (chmod(SOCKET_PATH, 0664) < 0) {
		syslog(LOG_ERR, "chmod(): %m");
		return -1;
	}

	if (listen(sock, SOCKET_BACKLOG)) {
		syslog(LOG_ERR, "listen(): %m");
		return -1;
	}

	if (chown(SOCKET_PATH, uid, gid) < 0) {
		syslog(LOG_ERR, "chown(): %m");
		return -1;
	}

	signal(SIGPIPE, SIG_IGN);

	return 0;
}

// Destroy the socket

void socket_destroy() {
	close(sock);
	unlink(SOCKET_PATH);
}

// Dispatching loop

void socket_loop() {
	int peer, bytes;
	char path[BUFFER_LENGTH];

	while (1) {
		peer = accept(sock, NULL, 0);

		if (peer < 0) {
			syslog(LOG_ERR, "accept(): %m");
			continue;
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
			player_state_t state = player_state(path);

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

			case ENGINEER:
				send(peer, "ENGINEER", 8, 0);
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
}

// Play a list of files

static int play(char *arg, int loop) {
	char **playlist;
	int i, n = atoi(arg);

	if (n < 1) {
		syslog(LOG_ERR, "Playlist size < 1");
		return -1;
	}

	playlist = (char **)malloc(sizeof(char *) * n);
	
	if (!playlist)
		return -1;
	
	arg = strtok(arg, " ");

	for (i = 0; i < n; i++) {
		arg = strtok(NULL, " ");

		if (arg) {
			playlist[i] = malloc(strlen(arg) + 1);
			
			if (!playlist[i]) {
				while (i > 0)
					free(playlist[--i]);
				
				free(playlist);
				return -1;
			}
			
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
