/*
 * Extension of output for GPIO
 * Victor Manuel Fernandez Castro
 * 18 August 2015
 */

#ifndef GPIO_H
#define GPIO_H

#include "output.h"

#define GPIO_BASE 0x20200000
#define GPIO_LENGTH 0x80

// We divide by 4 because sum of pointers is base 4 (sizeof(int))

#define OFFSET_SET0 7
#define OFFSET_SET1 8
#define OFFSET_CLR0 10
#define OFFSET_CLR1 11
#define OFFSET_LEVEL0 13
#define OFFSET_LEVEL1 14

typedef struct gpio_t {
	unsigned int gpfsel[6];
	unsigned int _reserved1;
	unsigned int gpset[2];
	unsigned int _reserved2;
	unsigned int gpclr[2];
	unsigned int _reserved3;
	unsigned int gplev[2];
} gpio_t;

/*
 * Track 0: 7 keys, starting at C4 (baroque keyboard)
 * Track 1: 7 keys, starting at C4 (romantic keyboard)
 * Track 2: 7 keys, starting at C4 (pedals)
 * Track 3: 7 keys, starting at C4 (stops)
 */

#define OUTPUT_NTRACKS  4			// Number of tracks
#define OUTPUT_LENGTH 7				// Length of each register
#define OUTPUT_OFFSET 60			// Base MIDI note

#define PIN_PORTS { 2, 3, 4, 17 }	// GPIO ports for output
#define PIN_RCKL 27					// Register clock
#define PIN_SRCKL 22				// Shifting clock

enum gpio_function {GPIO_INPUT, GPIO_OUTPUT};

#endif