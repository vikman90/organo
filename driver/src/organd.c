/*
 * Daemon for listening requests
 * Victor Manuel Fernandez Castro
 * 1 August 2015
 */

#include <syslog.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pwd.h>
#include <grp.h>
#include "peripherals.h"
#include "socket.h"
#include "player.h"
#include "output.h"
#include "values.h"
#include "uart.h"

// Cleanup function, called automatically on exiting.
static void cleanup();

// Action on SIGTERM
static void onsigterm();

// Setup function. Returns 0 on success, or -1 on error.
static int setup();

// Main function

int main() {
	if (setup() < 0)
		return EXIT_FAILURE;

	uart_loop();
	socket_loop();

	return EXIT_SUCCESS;
}

// Cleanup function, called automatically on exiting.

void cleanup() {
	player_stop();
	periph_destroy();
	output_destroy();
	uart_destroy();
	socket_destroy();
	player_destroy();
	closelog();
}

// Action on SIGTERM

void onsigterm() {
	exit(EXIT_SUCCESS);
}

// Setup function. Returns 0 on success, or -1 on error.

int setup() {	
	struct passwd *passwd = getpwnam(DAEMON_USER);
	
	if (passwd == NULL) {
		syslog(LOG_ERR, "getpwnam(): %m");
		return -1;
	}

	// Logging

	openlog(LOG_IDENT, 0, LOG_DAEMON);

	// Daemon

	if (daemon(0, 0) < 0) {
		syslog(LOG_ERR, "daemon(): %m");
		return -1;
	}

	atexit(cleanup);
	signal(SIGTERM, onsigterm);
	
	// Player
	
	if (player_init() < 0) {
		syslog(LOG_ERR, "Error at player_init()");
		return -1;
	}

	// Socket

	if (socket_init(passwd->pw_uid, passwd->pw_gid) < 0) {
		syslog(LOG_ERR, "Error at socket_init()");
		return -1;
	}

	// Output

	if (output_init() < 0) {
		syslog(LOG_ERR, "Error at output_init()");
		return -1;
	}

	// UART

	if (uart_init() < 0) {
		syslog(LOG_ERR, "Error at uart_init()");
		return -1;
	}

	// Peripherals

	if (periph_init() < 0) {
		syslog(LOG_ERR, "Error at periph_init()");
		return -1;
	}

	// Change UID and GID
	
	if (initgroups(passwd->pw_name, passwd->pw_gid) < 0) {
		syslog(LOG_ERR, "initgroups(): %m");
		return -1;
	}

	if (setgid(passwd->pw_gid) < 0) {
		syslog(LOG_ERR, "setgid(): %m");
		return -1;
	}

	if (setuid(passwd->pw_uid) < 0) {
		syslog(LOG_ERR, "setuid(): %m");
		return -1;
	}

	return 0;
}
