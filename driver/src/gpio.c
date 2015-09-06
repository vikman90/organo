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
#include <pthread.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "values.h"

typedef struct gpio_t {
	unsigned int gpfsel[6];
	unsigned int _reserved1;
	unsigned int gpset[2];
	unsigned int _reserved2;
	unsigned int gpclr[2];
} gpio_t;

enum gpio_function {GPIO_INPUT, GPIO_OUTPUT};

static const char PORTS[] = PIN_PORTS;							// GPIO ports
static const struct timespec PULSE_WIDTH = GPIO_PULSE_WIDTH;	// Pulse width for clock
static const struct timespec BUZZER_PULSE = METRONOME_PULSE;	// Duration of metronome click

static int metro_enabled = 0;
static pthread_t metro_thread;
static sem_t metro_semaphore;

static volatile gpio_t *gpio;						// GPIO base address
static char state[OUTPUT_LENGTH][OUTPUT_NTRACKS];	// Matrix of LENGTH rows and NTRACKS columns

// Select GPIO pin direction
static void gpio_fsel(unsigned int pin, enum gpio_function func);

// Metronome thread
static void* metronome_run(void *arg);

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
	gpio_fsel(PIN_BUZZER, GPIO_OUTPUT);

	for (i = 0; i < OUTPUT_NTRACKS; i++)
		gpio_fsel(PORTS[i], GPIO_OUTPUT);
	
	sem_init(&metro_semaphore, 0, 0);
	return pthread_create(&metro_thread, NULL, metronome_run, NULL);
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
	char *p = &state[0][0];

	for (i = 0; i < OUTPUT_LENGTH; i++) {
		setmask = clearmask = 0;

		for (j = 0; j < OUTPUT_NTRACKS; j++) {
			setmask |= *p << PORTS[j];
			clearmask |= !*(p++) << PORTS[j];
		}

		// Dump
		*gpio->gpset = setmask;
		*gpio->gpclr = clearmask;

		// Pulse on SRCKL
		*gpio->gpset = 1 << PIN_SRCKL;
		nanosleep(&PULSE_WIDTH, NULL);
		*gpio->gpclr = 1 << PIN_SRCKL;
	}

	// Pulse on RCKL
	*gpio->gpset = 1 << PIN_RCKL;
	nanosleep(&PULSE_WIDTH, NULL);
	*gpio->gpclr = 1 << PIN_RCKL;
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

// Metronome click

void output_metronome() {
	if (metro_enabled)
		sem_post(&metro_semaphore);
}

// Enable or disable metronome

void output_metronome_enable(int enabled) {
	metro_enabled = enabled;
}

// Query metronome state

int output_metronome_enabled() {
	return metro_enabled;
}

// Select GPIO pin direction

void gpio_fsel(unsigned int pin, enum gpio_function func)
{
	volatile unsigned int *reg = &gpio->gpfsel[pin / 10];
	int shift = pin % 10 * 3;

	*reg = (*reg & ~(7 << shift)) | (func << shift);
}

// Metronome thread

void* metronome_run(void __attribute__((unused)) *arg) {
	while (1) {
		sem_wait(&metro_semaphore);
		*gpio->gpset = 1 << PIN_BUZZER;
		nanosleep(&BUZZER_PULSE, NULL);
		*gpio->gpclr = 1 << PIN_BUZZER;
	}
	
	return NULL;
}
