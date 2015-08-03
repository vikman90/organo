/*
 * User interface for organ
 * Victor Manuel Fernandez Castro
 * 3 August 2015
 */
 
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFFER_LENGTH 256

static const char SOCKET_PATH[] = "/run/organd.sock";
static const char SYNTAX_FMT[] = "Sintaxis: %s play <archivo> | pause | resume | stop | status\n";

int main(int argc, char **argv) {
	int sock;
	struct sockaddr_un addr;
	char buffer[BUFFER_LENGTH];
	
	if (argc < 2) {
		fprintf(stderr, SYNTAX_FMT, *argv);
		return EXIT_FAILURE;
	}
	
	sock = socket(AF_UNIX, SOCK_STREAM, 0);

	if (sock < 0) {
		perror("socket()");
		return EXIT_FAILURE;
	}
	
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, SOCKET_PATH);

	if (connect(sock, (const struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("connect()");
		return EXIT_FAILURE;
	}
	
	if (!strcmp(argv[1], "pause")) {
		send(sock, "PAUSE", 5, 0);
		recv(sock, buffer, BUFFER_LENGTH, 0);
		
		if (strncmp(buffer, "OK", 2))
			fprintf(stderr, "Error en el controlador.\n");
		
	} else if (!strcmp(argv[1], "resume")) {
		send(sock, "RESUME", 6, 0);
		recv(sock, buffer, BUFFER_LENGTH, 0);
		
		if (strncmp(buffer, "OK", 2))
			fprintf(stderr, "Error en el controlador.\n");
	}  else if (!strcmp(argv[1], "stop")) {
		send(sock, "STOP", 4, 0);
		recv(sock, buffer, BUFFER_LENGTH, 0);
		
		if (strncmp(buffer, "OK", 2))
			fprintf(stderr, "Error en el controlador.\n");
	} else if (!strcmp(argv[1], "status")) {
		send(sock, "STATUS", 6, 0);
		recv(sock, buffer, BUFFER_LENGTH, 0);
		
		if (!strncmp(buffer, "PLAYING", 7))
			printf("Estado: reproduciendo.\n");
		else if (!strncmp(buffer, "PAUSED", 6))
			printf("Estado: en pausa.\n");
		else if (!strncmp(buffer, "STOPPED", 7))
			printf("Estado: detenido.\n");
		else
			printf("Estado: %s\n", buffer);
	}
	
	close(sock);
	return EXIT_SUCCESS;
}