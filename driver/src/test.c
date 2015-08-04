// Score testing on virtual monitor
// Victor Manuel Fernandez Castro
// 31 July 2015
// 3 August 2015

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "player.h"
#include "output.h"
#include "database.h"

int main(int argc, char **argv) {
	score_t *score = (score_t *)malloc(sizeof(score_t));;

	if (argc < 2) {
		fprintf(stderr, "Sintaxis: %s <archivo>\n", *argv);
		return EXIT_FAILURE;
	}
	
	if (output_init()) {
		perror("No se pudo inicializar la salida");
		return EXIT_FAILURE;
	}

	score->idscore = 0;
	score->path = (char *)malloc(strlen(argv[1]) + 1);
	strcpy(score->path, argv[1]);
	score->file = malloc(sizeof(midifile_t));

	if (midifile_init(score->file, argv[1]) < 0) {
		perror("No se pudo leer el archivo");
		return EXIT_FAILURE;
	}

	if (player_start(score, 1, -1, -1, 0)) {
		perror("No se pudo ejecutar el reproductor");
		return EXIT_FAILURE;
	}
	
	if (player_wait() < 0)
		perror("player_wait()");

	output_destroy();
	return EXIT_SUCCESS;
}
