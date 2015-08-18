/*
 * Extension of output for GPIO
 * Victor Manuel Fernandez Castro
 * 18 August 2015
 */

#ifndef GPIO_H
#define GPIO_H

#include "output.h"



/*
 * Track 0: 7 keys, starting at C4 (baroque keyboard)
 * Track 1: 7 keys, starting at C4 (romantic keyboard)
 * Track 2: 7 keys, starting at C4 (pedals)
 * Track 3: 7 keys, starting at C4 (stops)
 */

#define OUTPUT_NTRACKS  4			// Number of tracks
#define OUTPUT_LENGTH 7				// Length of each register
#define OUTPUT_OFFSET 60			// Base MIDI note

#endif