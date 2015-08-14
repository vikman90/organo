/*
 * Remote Receiver from UART
 * Victor Manuel Fernandez Castro
 * 14 August 2015
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <strings.h>
#include <termios.h>
#include <unistd.h>
#include <syslog.h>
#include <fcntl.h>
#include <stdio.h>

#define BAUD_RATE B9600
#define UART_BUFFER_MAX 16
#define UART_BUFFER_MIN 10
#define SOCK_BUFFER_MAX 4096

static const char UART_PATH[] = "/dev/ttyAMA0";
static const char SOCKET_PATH[] = "/run/organd.sock";
static const char CONFIG_PATH[] = "/etc/organ/remote.conf";

static struct termios oldtio;
static int tty;

// Play a list
static void uart_playlist(int list);

// Pause or resume
static void uart_pause();

// Stop playback
static void uart_stop();

// Open the socket and return socket descriptor (or -1 if error)
static int socket_open();

// Initialization of the serial port

int uart_init() {
	tty = open(UART_PATH, O_RDONLY | O_NOCTTY);
	struct termios termios;

	if (tty < 0) {
		syslog(LOG_ERR, "open(): %m");
		return -1;
	}

	tcgetattr(tty, &oldtio);
	bzero(&termios, sizeof(termios));
	termios.c_cc[VMIN] = UART_BUFFER_MIN;
	cfsetispeed(&termios, BAUD_RATE);
	tcsetattr(tty, TCSANOW, &termios);

	return 0;
}

// Destroy the connection

void uart_destroy() {
	tcsetattr(tty, TCSANOW, &oldtio);
	close(tty);
}

// Dispatching loop

void uart_loop() {
	char buffer[UART_BUFFER_MAX] = { 0 };

	while (1) {
		int n = read(tty, buffer, UART_BUFFER_MAX);
		
		if (n < 0) {
			syslog(LOG_ERR, "read(): %m");
			continue;
		}
		
		switch (buffer[7]) {
		case 'a':
			syslog(LOG_WARNING, "Battery low\n");
		case 'A':
			uart_playlist(0);
			break;
		case 'b':
			syslog(LOG_WARNING, "Battery low\n");
		case 'B':
			uart_playlist(1);
			break;
		case 'd':
			syslog(LOG_WARNING, "Battery low\n");
		case 'D':
			uart_pause();
			break;
		case 'h':
			syslog(LOG_WARNING, "Battery low\n");
		case 'H':
			uart_stop();
			break;
		}
	}
}

// Play a list

void uart_playlist(int list) {
	int sock, i, n;
	FILE *file = fopen(CONFIG_PATH, "r");
	char *next;
	char filebuf[SOCK_BUFFER_MAX];
	char sockbuf[SOCK_BUFFER_MAX];
	
	if (!file) {
		syslog(LOG_ERR, "fopen(): %m");
		return;
	}
	
	for (i = 0; i < list; i++) {
		if (!fgets(filebuf, SOCK_BUFFER_MAX, file)) {
			syslog(LOG_ERR, "End of file reached");
			fclose(file);
			return;
		}
	}
	
	fclose(file);
	next = strtok(filebuf, " ");
	
	if (!next) {
		syslog(LOG_WARNING, "Playlist empty");
		return;
	}
	
	for (n = 1; (next = strtok(NULL, " ")); n++);
	sprintf(sockbuf, "PLAYLOOP %d %s", n, filebuf);
	
	sock = socket_open();
	
	if (!sock)
		return;
	
	send(sock, sockbuf, strlen(sockbuf), 0);
	n = recv(sock, sockbuf, SOCK_BUFFER_MAX, 0);
	
	if (n < 0)
		syslog(LOG_ERR, "recv(): %m");
	
	sockbuf[n] = '\0';
	
	if (!strcmp(sockbuf, "OK"))
		syslog(LOG_ERR, "Daemon returned error.");
		
	close(sock);
}

// Pause or resume

void uart_pause() {
	char buffer[SOCK_BUFFER_MAX];
	int sock = socket_open();
	int n;
	
	if (!sock)
		return;
	
	send(sock, "STATUS", 6, 0);
	n = recv(sock, buffer, SOCK_BUFFER_MAX, 0);
	buffer[n] = '\0';
	
	if (!strncmp(buffer, "PLAYING", 7))
		send(sock, "PAUSE", 5, 0);
	else if (!strncmp(buffer, "PAUSED", 6))
		send(sock, "RESUME", 6, 0);
	else {
		// Stopped
		close(sock);
		return;
	}
	
	recv(sock, buffer, SOCK_BUFFER_MAX, 0);
	
	if (strncmp(buffer, "OK", 2))
		syslog(LOG_ERR, "Daemon returned error.");
	
	close(sock);
}

// Stop playback

void uart_stop() {
	char buffer[SOCK_BUFFER_MAX];
	int sock = socket_open();
	
	if (!sock)
		return;
	
	send(sock, "STOP", 4, 0);
	recv(sock, buffer, SOCK_BUFFER_MAX, 0);
	
	if (strncmp(buffer, "OK", 2))
		syslog(LOG_ERR, "Daemon returned error.");
	
	close(sock);
}

// Open the socket and return socket descriptor (or -1 if error)

int socket_open() {
	int sock;
	struct sockaddr_un addr;
	sock = socket(AF_UNIX, SOCK_STREAM, 0);

	if (sock < 0) {
		syslog(LOG_ERR, "socket(): %m");
		return -1;
	}
	
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, SOCKET_PATH);

	if (connect(sock, (const struct sockaddr *)&addr, sizeof(addr)) < 0) {
		syslog(LOG_ERR, "connect(): %m");
		return -1;
	}
	
	return sock;
}
