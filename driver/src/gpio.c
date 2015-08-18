/*
 * GPIO output for prototype board
 * Implementation of output_* interface
 * Victor Manuel Fernandez Castro
 * 31 July 2015
 *
 * Target device:
 * Raspberry Pi B - BCM2835
 * Custom organ interpreter board with SN74HC595D shifting registers
 */

#include <stdlib.h>
#include <string.h>
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

static const char PORTS[] = { 2, 3, 4, 17 };			// GPIO ports
static char state[LENGTH][NTRACKS];						// Matrix of LENGTH rows and NTRACKS columns

// Select GPIO pin direction
static void gpio_fsel(unsigned int pin, enum gpio_function func);

// Very short delay
static void delay();

// Initialize output

int output_init() {
	int fd = open("/dev/mem", O_RDWR);
	int i;

	if (fd < 0)
		return -1;

	// Map GPIO physical address

	gpio_addr = (unsigned int *)mmap(NULL, GPIO_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO_BASE);

	if (gpio_addr == MAP_FAILED)
		return -1;

	bzero(state, LENGTH * NTRACKS);

	// Set GPIO function

	gpio_fsel(RCKL, GPIO_OUTPUT);
	gpio_fsel(SRCKL, GPIO_OUTPUT);

	for (i = 0; i < NTRACKS; i++)
		gpio_fsel(PORTS[i], GPIO_OUTPUT);

	return 0;
}

// Release output

void output_destroy() {
	munmap((void *)gpio_addr, GPIO_LENGTH);
}

// Play note

void output_noteon(int track, int note) {
	note -= OFFSET;

	if (track < NTRACKS && note >= 0 && note < LENGTH)
		state[note][track] = 1;
}

// Stop note

void output_noteoff(int track, int note) {
	note -= OFFSET;

	if (track < NTRACKS && note >= 0 && note < LENGTH)
		state[note][track] = 0;
}

// Dump data into output

void output_update() {
	int i, j;
	unsigned int setmask, clearmask;

	for (i = LENGTH - 1; i >= 0; i--) {
		setmask = clearmask = 0;

		for (j = 0; j < NTRACKS; j++) {
			setmask |= state[i][j] << PORTS[j];
			clearmask |= !state[i][j] << PORTS[j];
		}

		// Dump
		*(gpio_addr + OFFSET_SET0) = setmask;
		*(gpio_addr + OFFSET_CLR0) = clearmask;

		// Pulse on SRCKL
		*(gpio_addr + OFFSET_SET0) = 1 << SRCKL;
		delay();
		*(gpio_addr + OFFSET_CLR0) = 1 << SRCKL;
	}

	// Pulse on RCKL
	*(gpio_addr + OFFSET_SET0) = 1 << RCKL;
	delay();
	*(gpio_addr + OFFSET_CLR0) = 1 << RCKL;
}

// Silence every note and reset device

void output_panic() {
	bzero(state, LENGTH * NTRACKS);
	output_update();
}

// Silence every note, keeping device's state

void output_silence() {
	char stack[LENGTH][NTRACKS] = { { 0 } };

	memcpy(stack, state, LENGTH * NTRACKS);
	bzero(state, LENGTH * NTRACKS);
	output_update();
	memcpy(state, stack, LENGTH * NTRACKS);
}

// Select GPIO pin direction

void gpio_fsel(unsigned int pin, enum gpio_function func)
{
	volatile unsigned int *reg = gpio_addr + pin / 10;
	int shift = pin % 10 * 3;

	*reg = (*reg & ~(7 << shift)) | (func << shift);
}

// Very short delay

void delay() {

	// SN74HC595D tpd = 13 ns
	// 10 instr * ( 1 / 700 MHz ) >= 14.2 ns

	asm ("nop\n\t"
	     "nop\n\t"
		 "nop\n\t"
		 "nop\n\t"
		 "nop\n\t"
		 "nop\n\t"
		 "nop\n\t"
		 "nop\n\t"
		 "nop\n\t"
		 "nop");
}
