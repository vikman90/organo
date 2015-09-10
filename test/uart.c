/*
 * Remote Receiver from UART
 * Victor Manuel Fernandez Castro
 * 13 August 2015
 */

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <strings.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <stdio.h>

#define BAUD_RATE B9600
#define BUFFER_LENGTH 10

static const char UART[] = "/dev/ttyAMA0";
static const char SERIAL[] = "0016D34";
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
	termios.c_cflag = CS7 | CLOCAL | CREAD;
	termios.c_iflag = IGNPAR;
	termios.c_cc[VMIN] = BUFFER_LENGTH;
	cfsetispeed(&termios, BAUD_RATE);
	tcflush(tty, TCIFLUSH);
	tcsetattr(tty, TCSANOW, &termios);

	return 0;
}

void uart_destroy() {
	tcsetattr(tty, TCSANOW, &oldtio);
	close(tty);
}

int main() {
	char buffer[BUFFER_LENGTH] = { 0 };
	signal(SIGTERM, uart_destroy);

	if (uart_init() < 0)
		return EXIT_FAILURE;

	while (1) {
		int n = read(tty, buffer, BUFFER_LENGTH);

		if (n < 1) {
			printf("Error\n", n);
			return 1;
		}

		if (strncmp(&buffer[8], "\r\n", 2)) {
			printf("Error de recepcion.\n");
			tcflush(tty, TCIFLUSH);
		} else if (strncmp(buffer, SERIAL, 7))
			printf("No coincide el numero de serie\n");
		else
			printf("Funcion %c (%d)\n", buffer[7], n);
	}
}
