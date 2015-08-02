#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>

#define BUFFER_LENGTH 256

static const char SOCKET_PATH[] = "/run/organd.sock";
static char buffer[BUFFER_LENGTH];

int main() {
	int sock = socket(AF_UNIX, SOCK_STREAM, 0);
	struct sockaddr_un addr;
	
	if (sock < 0) {
		perror("socket()");
		return 1;
	}
	
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, SOCKET_PATH);
	
	if (connect(sock, (const struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("connect()");
		return 1;
	}
	
	if (send(sock, "PING", 4, 0) < 0) {
		perror("send()");
		return 1;
	}
	
	int length = recv(sock, buffer, BUFFER_LENGTH, 0);
	buffer[length] = 0;
	printf("%s\n", buffer);
	
	return 0;
}