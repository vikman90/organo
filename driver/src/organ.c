/*
 * User interface for organ
 * Victor Manuel Fernandez Castro
 * 3 August 2015
 */

#include <sys/stat.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#define BUFFER_LENGTH 256

static const char SOCKET_PATH[] = "/run/organd.sock";
static const char SYNTAX_FMT[] = "Sintaxis: %s play[loop] <archivo> | pause | resume | stop | status\n";

int main(int argc, char **argv) {
	int sock, n;
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
	
	if (!strncmp(argv[1], "play", 4)) {
		int fd;
		char path[BUFFER_LENGTH];
		
		if (argc < 3) {
			fprintf(stderr, SYNTAX_FMT, *argv);
			return EXIT_FAILURE;
		}
		
		// Get absolute path
		
		if (argv[2][1] == '/')
			strcpy(path, argv[2]);
		else {
			getcwd(path, BUFFER_LENGTH);
			strcat(path, "/");
			strcat(path, argv[2]);
		}
		
		fd = open(path, O_RDONLY);
		
		if (fd < 0) {
			fprintf(stderr, "No se pudo encontrar %s\n", path);
			return EXIT_FAILURE;
		} else
			close(fd);
		
		if (!strcmp(argv[1], "playloop"))
			sprintf(buffer, "PLAYLOOP 1 %s", path);
		else
			sprintf(buffer, "PLAY 1 %s", path);
		
		send(sock, buffer, strlen(buffer), 0);
		n = recv(sock, buffer, BUFFER_LENGTH, 0);

		if (strncmp(buffer, "OK", 2)) {
			buffer[n] = '\0';
			fprintf(stderr, "Error en el controlador.\n");
			printf("Devuelve: %s\n", buffer);
		}
	} else if (!strcmp(argv[1], "pause")) {
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
		n = recv(sock, buffer, BUFFER_LENGTH, 0);
		buffer[n] = '\0';
		
		if (!strncmp(buffer, "PLAYING", 7))
			printf("Estado: reproduciendo %s\n", buffer + 8);
		else if (!strncmp(buffer, "PAUSED", 6))
			printf("Estado: en pausa, archivo %s\n", buffer + 8);
		else if (!strncmp(buffer, "STOPPED", 7))
			printf("Estado: detenido.\n");
		else
			printf("Estado: %s\n", buffer);
	} else {
		fprintf(stderr, SYNTAX_FMT, *argv);
	}
	
	close(sock);
	return EXIT_SUCCESS;
}