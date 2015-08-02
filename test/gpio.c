#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#define BUFFER_SIZE 32

typedef struct gpio_t
{
	int n;
	int direction;
	int value;
	int edge;
	int active_low;
} gpio_t;

gpio_t pin;

static int gpio_init(int n, gpio_t *reg)
{
	char buffer[BUFFER_SIZE];
/*	int fd = open("/sys/class/gpio/export", O_WRONLY);

	if (fd < 0) {
		perror("export");
		return -1;
	}

	if (dprintf(fd, "%d\n", n) < 1) {
		fprintf(stderr, "dprintf() < 1\n");
		return -1;
	}

	close(fd);
*/
	reg->n = n;
	sprintf(buffer, "/sys/class/gpio/gpio%d/direction", n);

	if ((reg->direction = open(buffer, O_RDWR)) < 0) {
		perror("buffer");
		return -1;
	}
	sprintf(buffer, "/sys/class/gpio/gpio%d/value", n);

	if ((reg->value = open(buffer, O_RDWR | O_NONBLOCK)) < 1) {
		perror("buffer");
		return -1;
	}

	sprintf(buffer, "/sys/class/gpio/gpio%d/edge", n);
	reg->edge = open(buffer, O_RDWR);
	sprintf(buffer, "/sys/class/gpio/gpio%d/active_low", n);
	reg->active_low = open(buffer, O_RDWR);

	return 0;
}

static void gpio_finalize(gpio_t *reg)
{
//	int fd = open("/sys/class/gpio/unexport", O_WRONLY);

	close(reg->direction);
	close(reg->value);
	close(reg->edge);
	close(reg->active_low);

/*	dprintf(fd, "%d\n", reg->n);
	close(fd);
*/
}

static void onexit()
{
	gpio_finalize(&pin);
	printf("onexit()\n");
}

static void onsigint()
{
	printf("onsigint()\n");
	exit(0);
}

int main()
{
	int epfd;
	char buffer[BUFFER_SIZE];
	struct epoll_event req;
	struct epoll_event events;

	if (gpio_init(3, &pin) < 0) {
		fprintf(stderr, "Error en gpio_init()\n");
		return EXIT_FAILURE;
	}

	atexit(onexit);
	signal(SIGINT, onsigint);

	epfd = epoll_create(1);
	req.events = EPOLLPRI;
	req.data.fd = pin.value;

	write(pin.direction, "in\n", 3);
	write(pin.edge, "rising\n", 7);

	if (epoll_ctl(epfd, EPOLL_CTL_ADD, pin.value, &req) < 0) {
		perror("Error en epoll_ctl()");
		return EXIT_FAILURE;
	}

	while (1) {
		int n = epoll_wait(epfd, &events, 1, -1);

		if (n) {
			lseek(pin.value, 0, SEEK_SET);
			int count = read(pin.value, buffer, BUFFER_SIZE);
			buffer[count] = '\0';
			printf("Interrupcion. Lectura: %s\n", buffer);
		} else
			printf("Tiempo expirado\n");
	}

	return EXIT_SUCCESS;
}
