/*
 * Database interface
 * Victor Manuel Fernandez Castro
 * 2 August 2015
 */

#ifndef DATABASE_H
#define DATABASE_H

#include "midi.h"

typedef struct score_t {
	int idscore;
	char *path;
	midifile_t *file;
} score_t;

// Initializes the database. Returns 0 on success or -1 on error
int database_init();

// Close a database connection
void database_destroy();

// Queries all scores from a playlist, returns number of scores or -1 on error
int database_query(score_t **scores, int idplaylist);

// Delete an array of scores
void score_destroy(score_t *scores, int n);

#endif
