// Score testing on virtual monitor
// Victor Manuel Fernandez Castro
// 31 July 2015

#include <stdlib.h>
#include <stdio.h>
#include "midi.h"
#include "player.h"
#include "output.h"

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
	
	if (output_init()) {
		perror("No se pudo inicializar la salida");
		return EXIT_FAILURE;
	}
	
	if (player_start(&score)) {
		perror("No se pudo ejecutar el reproductor");
		return EXIT_FAILURE;
	}
	
	player_wait();
	output_destroy();
	return EXIT_SUCCESS;
}