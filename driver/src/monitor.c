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

#define NTRACKS  4

static const char OFFSET[] = { 36, 36, 24, 60 };
static const char LENGTH[] = { 48, 48, 12, 26 };

static unsigned int notes;
static char *state;
static char **channel;

int output_init() {
	unsigned int i;
	notes = 0;
	
	for (i = 0; i < NTRACKS; i++)
		notes += LENGTH[i];
	
	state = (char *)malloc(notes * sizeof(char));
	channel = (char **)malloc(NTRACKS * sizeof(char *));
	
	if (!(state && channel))
		return -1;
	
	notes = 0;
	
	for (i = 0; i < NTRACKS; i++) {
		channel[i] = &state[notes];
		notes += LENGTH[i];
	}
	
	return 0;
}

void output_destroy() {
	free(state);
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

void output_panic() {
	memset(state, 0, notes);
	output_update();
}
