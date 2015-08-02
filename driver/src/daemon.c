/*
 * Daemon for listening requests
 * Victor Manuel Fernandez Castro
 * 1 August 2015
 */

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>

#define BUFFER_LENGTH 256	// Length of receiving buffer
#define BACKLOG 5			// Listening queue length

static const char PID_PATH[] = "/run/organd.pid";		// Path for pid file
static const char SOCKET_PATH[] = "/run/organd.sock";	// Path for socket file

static char buffer[BUFFER_LENGTH];

// Cleanup function, called automatically on exiting.

static void cleanup() {
	unlink(PID_PATH);
	unlink(SOCKET_PATH);
}

// Setup function. Returns socket id, or -1 on error.

static int setup() {
	int fd;
	int sock;
	struct sockaddr_un addr;
	
	if (daemon(0, 0) < 0) {
		perror("daemon()");
		return -1;
	}

	atexit(cleanup);
	fd = open(PID_PATH, O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
	
	if (fd < 0) {
		perror("open()");
		return -1;
	}
	
	dprintf(fd, "%d", getpid());
	close(fd);	
	sock = socket(AF_UNIX, SOCK_STREAM, 0);

	if (sock < 0) {
		perror("socket()");
		return -1;
	}
	
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, SOCKET_PATH);
	
	if (bind(sock, (const struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("bind()");
		return -1;
	}
	
	if (listen(sock, BACKLOG)) {
		perror("listen()");
		return -1;
	}
	
	return sock;
}

int main() {
	int peer;
	int length;
	int sock = setup();
	
	if (sock < 0)
		return EXIT_FAILURE;
	
	while (1) {
		peer = accept(sock, NULL, 0);
		
		if (peer < 0) {
			perror("accept()");
			return EXIT_FAILURE;
		}
		
		length = recv(peer, buffer, BUFFER_LENGTH, 0);
		
		if (!strcmp(buffer, "PING")) {
			printf("Recibido PING\n");
			send(peer, "OLA K ASE", 10, 0);
		}
		else if (!strcmp(buffer, "EXIT")) {
			printf("Recibido EXIT\n");
			break;
		} else {
			buffer[length] = 0;
			printf("Recbido: %s\n", buffer);
		}
	}
	
	destroy(sock);
	return EXIT_SUCCESS;
}