// Midi file information
// Victor Manuel Fernandez Castro
// 30 July 2015

#include <stdlib.h>
#include <stdio.h>
#include "midi.h"


int main(int argc, char **argv) {
	midifile_t score;
	
	if (argc < 2) {
		fprintf(stderr, "Sintaxis: %s <archivo>\n", *argv);
		return EXIT_FAILURE;
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
	
	return EXIT_SUCCESS;
}