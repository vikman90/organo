// Score testing on virtual monitor
// Victor Manuel Fernandez Castro
// 31 July 2015
// 3 August 2015

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "player.h"
#include "output.h"

int main(int argc, char **argv) {
	char *path;
	if (argc < 2) {
		fprintf(stderr, "Sintaxis: %s <archivo>\n", *argv);
		return EXIT_FAILURE;
	}
	
	if (output_init()) {
		perror("No se pudo inicializar la salida");
		return EXIT_FAILURE;
	}

	path = (char *)malloc(strlen(argv[1]) + 1);
	strcpy(path, argv[1]);

	if (player_start(&path, 1, 0)) {
		perror("No se pudo ejecutar el reproductor");
		return EXIT_FAILURE;
	}
	
	if (player_wait() < 0)
		perror("player_wait()");

	output_destroy();
	return EXIT_SUCCESS;
}
