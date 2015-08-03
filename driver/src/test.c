// Score testing on virtual monitor
// Victor Manuel Fernandez Castro
// 31 July 2015

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "player.h"
#include "output.h"
#include "database.h"

int main(int argc, char **argv) {
	score_t *scores = (score_t *)malloc(sizeof(score_t));;
	
	if (argc < 2) {
		fprintf(stderr, "Sintaxis: %s <archivo>\n", *argv);
		return EXIT_FAILURE;
	}
	
	if (output_init()) {
		perror("No se pudo inicializar la salida");
		return EXIT_FAILURE;
	}
	
	scores->idscore = 0;
	scores->path = argv[1];
	
	if (player_start(scores, 1, -1, -1, 0)) {
		perror("No se pudo ejecutar el reproductor");
		return EXIT_FAILURE;
	}
	
	player_wait();
	output_destroy();
	return EXIT_SUCCESS;
}