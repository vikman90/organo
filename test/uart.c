/*
 * Remote Receiver from UART
 * Victor Manuel Fernandez Castro
 * 13 August 2015
 */

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <strings.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <stdio.h>

#define BAUD_RATE B9600
#define BUFFER_MAX 16
#define BUFFER_MIN 10

static const char UART[] = "/dev/ttyAMA0";
static struct termios oldtio;
static int tty;

int uart_init() {
	tty = open(UART, O_RDONLY | O_NOCTTY);
	struct termios termios;

	if (tty < 0) {
		perror("open()");
		return -1;
	}

	tcgetattr(tty, &oldtio);
	bzero(&termios, sizeof(termios));
	termios.c_cc[VMIN] = BUFFER_MIN;
	cfsetispeed(&termios, BAUD_RATE);
	tcsetattr(tty, TCSANOW, &termios);

	return 0;
}

void uart_destroy() {
	tcsetattr(tty, TCSANOW, &oldtio);
	close(tty);
}

int main() {
	char buffer[BUFFER_MAX] = { 0 };
	signal(SIGTERM, uart_destroy);

	if (uart_init() < 0)
		return EXIT_FAILURE;

	while (1) {
		int n = read(tty, buffer, BUFFER_MAX);
		
		if (n < 0) {
			perror("read()");
			continue;
		}
		
		buffer[n] = '\0';
		printf("Leidos %d bytes: %s\n", n, buffer);
	}
}