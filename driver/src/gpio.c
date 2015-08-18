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
#include "gpio.h"

#define GPIO_BASE 0x20200000
#define GPIO_LENGTH 0x80

#define PIN_RCKL 27		// Register clock
#define PIN_SRCKL 22	// Shifting clock

typedef struct gpio_t {
	unsigned int gpfsel[6];
	unsigned int _reserved1;
	unsigned int gpset[2];
	unsigned int _reserved2;
	unsigned int gpclr[2];
	unsigned int _reserved3;
	unsigned int gplev[2];
} gpio_t;

enum gpio_function {GPIO_INPUT, GPIO_OUTPUT};

static const char PORTS[] = { 2, 3, 4, 17 };		// GPIO ports

static volatile gpio_t *gpio;						// GPIO base address
static char state[OUTPUT_LENGTH][OUTPUT_NTRACKS];	// Matrix of LENGTH rows and NTRACKS columns

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

	gpio = (gpio_t *)mmap(NULL, GPIO_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO_BASE);

	if (gpio == MAP_FAILED)
		return -1;

	bzero(state, OUTPUT_LENGTH * OUTPUT_NTRACKS);

	// Set GPIO function

	gpio_fsel(PIN_RCKL, GPIO_OUTPUT);
	gpio_fsel(PIN_SRCKL, GPIO_OUTPUT);

	for (i = 0; i < OUTPUT_NTRACKS; i++)
		gpio_fsel(PORTS[i], GPIO_OUTPUT);

	return 0;
}

// Release output

void output_destroy() {
	munmap((void *)gpio, GPIO_LENGTH);
}

// Play note

void output_noteon(int track, int note) {
	note -= OUTPUT_OFFSET;

	if (track < OUTPUT_NTRACKS && note >= 0 && note < OUTPUT_LENGTH)
		state[note][track] = 1;
}

// Stop note

void output_noteoff(int track, int note) {
	note -= OUTPUT_OFFSET;

	if (track < OUTPUT_NTRACKS && note >= 0 && note < OUTPUT_LENGTH)
		state[note][track] = 0;
}

// Dump data into output

void output_update() {
	int i, j;
	unsigned int setmask, clearmask;

	for (i = OUTPUT_LENGTH - 1; i >= 0; i--) {
		setmask = clearmask = 0;

		for (j = 0; j < OUTPUT_NTRACKS; j++) {
			setmask |= state[i][j] << PORTS[j];
			clearmask |= !state[i][j] << PORTS[j];
		}

		// Dump
		gpio->gpset[0] = setmask;
		gpio->gpclr[0] = clearmask;

		// Pulse on SRCKL
		gpio->gpset[0] = 1 << PIN_SRCKL;
		delay();
		gpio->gpclr[0] = 1 << PIN_SRCKL;
	}

	// Pulse on RCKL
	gpio->gpset[0] = 1 << PIN_RCKL;
	delay();
	gpio->gpclr[0] = 1 << PIN_RCKL;
}

// Silence every note and reset device

void output_panic() {
	bzero(state, OUTPUT_LENGTH * OUTPUT_NTRACKS);
	output_update();
}

// Silence every note, keeping device's state

void output_silence() {
	char stack[OUTPUT_LENGTH][OUTPUT_NTRACKS] = { { 0 } };

	memcpy(stack, state, OUTPUT_LENGTH * OUTPUT_NTRACKS);
	bzero(state, OUTPUT_LENGTH * OUTPUT_NTRACKS);
	output_update();
	memcpy(state, stack, OUTPUT_LENGTH * OUTPUT_NTRACKS);
}

// Select GPIO pin direction

void gpio_fsel(unsigned int pin, enum gpio_function func)
{
	volatile unsigned int *reg = &gpio->gpfsel[pin / 10];
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
