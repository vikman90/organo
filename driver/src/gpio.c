/*
 * GPIO output for prototype board
 * Implementation of output_* interface
 * Victor Manuel Fernandez Castro
 * 31 July 2015
 */

#include <stdlib.h>
#include <strings.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define GPIO_BASE 0x20200000
#define GPIO_LENGTH 0x80

// We divide by 4 because sum of pointers is base 4 (sizeof(int))

#define OFFSET_SET0 7
#define OFFSET_SET1 8
#define OFFSET_CLR0 10
#define OFFSET_CLR1 11
#define OFFSET_LEVEL0 13
#define OFFSET_LEVEL1 14

enum gpio_function {GPIO_INPUT, GPIO_OUTPUT};
static volatile unsigned int *gpio_addr;

/*
 * Track 0: 8 keys, starting at C4 (baroque keyboard)
 * Track 1: 8 keys, starting at C4 (romantic keyboard) 
 * Track 2: 8 keys, starting at C4 (pedals)
 * Track 3: 8 keys, starting at C4 (stops)
 */

#define NTRACKS  4	// Number of tracks
#define LENGTH 7	// Length of each register
#define OFFSET 60	// Base MIDI note
#define RCKL 27		// Register clock
#define SRCKL 22	// Shifting clock

static const char PORTS[] = { 2, 3, 4, 17 };
static char state[LENGTH][NTRACKS];	// Matrix of LENGTH rows and NTRACKS columns

static void gpio_fsel(unsigned int pin, enum gpio_function func);

int output_init() {
	int fd = open("/dev/mem", O_RDWR);
	int i;
	
	if (fd < 0)
		return -1;
	
	// Map GPIO physical address
	
	gpio_addr = (unsigned int *)mmap(NULL, GPIO_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO_BASE);
	
	if (gpio_addr == MAP_FAILED)
		return -1;
	
	// Set GPIO function
	
	gpio_fsel(RCKL, GPIO_OUTPUT);
	gpio_fsel(SRCKL, GPIO_OUTPUT);
	
	for (i = 0; i < NTRACKS; i++)
		gpio_fsel(PORTS[i], GPIO_OUTPUT);
	
	return 0;
}

void output_destroy() {
	munmap((void *)gpio_addr, GPIO_LENGTH);
}

void output_noteon(int track, int note) {
	note -= OFFSET;
	
	if (note >= 0 && note < LENGTH)
		state[note][track] = 1;
}

void output_noteoff(int track, int note) {
	note -= OFFSET;
	
	if (note >= 0 && note < LENGTH)
		state[note][track] = 0;
}

void output_update() {
	int i, j;
	unsigned int setmask, clearmask;
	
	for (i = LENGTH - 1; i >= 0; i--) {
		setmask = clearmask = 0;
		
		for (j = 0; j < NTRACKS; j++) {
			setmask |= state[i][j] & (1 << PORTS[j]);
			clearmask |= ~state[i][j] & (1 << PORTS[j]);
		}
		
		// Dump
		*(gpio_addr + OFFSET_SET0) = setmask;
		*(gpio_addr + OFFSET_CLR0) = clearmask;
		
		// Pulse on SRCKL
		*(gpio_addr + OFFSET_SET0) = (1 << SRCKL);
		*(gpio_addr + OFFSET_CLR0) = (1 << SRCKL);
	}
	
	// Pulse on RCKL
	*(gpio_addr + OFFSET_SET0) = (1 << RCKL);
	*(gpio_addr + OFFSET_CLR0) = (1 << RCKL);
}

void output_panic() {
	bzero(state, LENGTH * NTRACKS);
	output_update();
}

static void gpio_fsel(unsigned int pin, enum gpio_function func)
{
	*(gpio_addr + pin / 10) = func << pin % 10 * 3;
}
