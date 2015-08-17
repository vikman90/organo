// Midi file loading loop (for debugging)
// Victor Manuel Fernandez Castro
// 17 August 2015

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "midi.h"

static int duration(const char *path) {
	midifile_t score;
	
	if (midifile_init(&score, path)) {
		perror("No se pudo leer el archivo");
		return EXIT_FAILURE;
	}
	
	printf("%d\n", midifile_duration(&score));
	return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
	int i, n;
	midifile_t score;
	
	if (argc < 3) {
		fprintf(stderr, "Sintaxis: %s <archivo> <vueltas>\n", *argv);
		return EXIT_FAILURE;
	}
	
	n = atoi(argv[2]);
	
	for (i = 0; i < n; i++) {
		/*char **playlist = (char**)malloc(sizeof(char*));
		playlist[0] = (char*)malloc(sizeof(char) * strlen(argv[1]) + 1);
		strcpy(playlist[0], argv[1]);
		player_start(playlist, 1, 0);
		player_wait();*/
		if (midifile_init(&score, argv[1]) < 0)
			return 1;
		midifile_destroy(&score);
	}
	
	return EXIT_SUCCESS;
}
