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
int db_init();

// Close a database connection
void db_destroy();

// Queries all scores from a playlist, indexed by a shortcut id (returns number of scores or -1 on error)
int db_query(char ***scores, int idshortcut);

#endif
