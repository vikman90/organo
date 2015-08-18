/*
 * Virtual output for monitoring
 * Implementation of output_* interface
 * Victor Manuel Fernandez Castro
 * 31 July 2015
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*
 * Track 0: 48 keys, starting at C2 (baroque keyboard)
 * Track 1: 48 keys, starting at C2 (romantic keyboard) 
 * Track 2: 12 keys, starting at C1 (pedals)
 * Track 3: 26 keys, starting at C4 (stops)
 */

#define NOTES 134	// Number of controlled notes
#define NTRACKS 4	// Number of tracks

static const char OFFSET[] = { 36, 36, 24, 60 };
static const char LENGTH[] = { 48, 48, 12, 26 };

static char state[NOTES];	// Length: notes
static char **channel;		// Pointers to initial sections of each channel into state

int output_init() {
	unsigned int i;
	int offset = 0;

	channel = (char **)malloc(NTRACKS * sizeof(char *));
	
	if (!channel)
		return -1;
	
	for (i = 0; i < NTRACKS; i++) {
		channel[i] = &state[offset];
		offset += LENGTH[i];
	}
	
	return 0;
}

void output_destroy() {
	free(channel);
}

void output_noteon(int track, int note) {
	note -= OFFSET[track];
	
	if (track < NTRACKS && note >= 0 && note < LENGTH[track])
		channel[track][note] = 1;
}

void output_noteoff(int track, int note) {
	note -= OFFSET[track];
	
	if (track < NTRACKS && note >= 0 && note < LENGTH[track])
		channel[track][note] = 0;
}

void output_update() {
	int i;
	int j;
	
	for (i = 0; i < NTRACKS; i++) {
		printf("Pista %d: ", i);
		
		for (j = 0; j < LENGTH[i]; j++)
			printf("%hhd", channel[i][j]);
		
		printf("\n");
	}
	
	printf("\n");
}

void output_silence() {
	int i;
	int j;
	
	for (i = 0; i < NTRACKS; i++) {
		printf("Pista %d: ", i);
		
		for (j = 0; j < LENGTH[i]; j++)
			printf("0");
		
		printf("\n");
	}
	
	printf("\n");
}

void output_panic() {
	memset(state, 0, NOTES);
	output_update();
}
