/*
 * Daemon for listening remote Receiver from UART
 * Victor Manuel Fernandez Castro
 * 14 August 2015
 */

#include <syslog.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "uart.h"

static const char LOG_IDENT[] = "remoted";	// Logging identity

// Cleanup function, called automatically on exiting.

static void cleanup() {
	uart_destroy();
	closelog();
}

// Action on SIGTERM

static void onsigterm() {
	exit(EXIT_SUCCESS);
}

// Setup function. Returns 0 on success, or -1 on error.

static int setup(int uid, int gid) {

	// Logging

	openlog(LOG_IDENT, 0, LOG_DAEMON);

	// Daemon

	if (daemon(0, 0) < 0) {
		syslog(LOG_ERR, "daemon(): %m");
		return -1;
	}

	atexit(cleanup);
	signal(SIGTERM, onsigterm);

	// UART

	if (uart_init() < 0) {
		syslog(LOG_ERR, "Error at socket_init()");
		return -1;
	}

	// Change UID and GID

	if (setgid(gid) < 0) {
		syslog(LOG_ERR, "setgid(): %m");
		return -1;
	}

	if (setuid(uid) < 0) {
		syslog(LOG_ERR, "setuid(): %m");
		return -1;
	}

	return 0;
}

int main(int argc, char **argv) {
	if (argc < 3) {
		fprintf(stderr, "Syntax: %s <uid> <gid>", argv[0]);
		return EXIT_FAILURE;
	}
	
	if (setup(atoi(argv[1]), atoi(argv[2])) < 0)
		return EXIT_FAILURE;
	
	uart_loop();

	return EXIT_SUCCESS;
}
