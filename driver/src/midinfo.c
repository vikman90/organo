// Midi file information
// Victor Manuel Fernandez Castro
// 30 July 2015

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
	midifile_t score;
	
	if (argc < 2) {
		fprintf(stderr, "Sintaxis: %s [--duration] <archivo>\n", *argv);
		return EXIT_FAILURE;
	}
	
	if (!strcmp(argv[1], "--duration")) {
		if (argc < 3) {
			fprintf(stderr, "Sintaxis: %s [--duration] <archivo>\n", *argv);
			return EXIT_FAILURE;
		}
		
		return duration(argv[2]);
	}
	
	if (midifile_init(&score, argv[1])) {
		perror("No se pudo leer el archivo");
		return EXIT_FAILURE;
	}
	
	switch (score.format) {
	case SINGLE_TRACK:
		printf("Formato: Pista simple.\n");
		break;
	case MULTIPLE_SIMULTANEOUS:
		printf("Formato: Pistas simultaneas.\n");
		break;
	case MULTIPLE_INDEPENDENT:
		printf("Formato: Pistas independientes.\n");
		break;
	default:
		printf("Formato: desconocido");
	}

	printf("Pistas: %u\n", score.ntracks);
	printf("Division de tiempo: %u ticks/negra.\n", score.timediv);
	printf("Duracion: %d seg.\n", midifile_duration(&score));
	
	return EXIT_SUCCESS;
}
