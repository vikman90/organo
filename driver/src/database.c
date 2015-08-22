/*
 * Database access
 * Victor Manuel Fernandez Castro
 * 2 August 2015
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <mysql/mysql.h>
#include "values.h"

static MYSQL *conn;

// Initializes the database. Returns 0 on success or -1 on error

int db_init() {
	conn = mysql_init(NULL);

	if (conn == NULL)
		return -1;

	if (!mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 0, NULL, 0))
		return -1;

	return 0;
}

// Close a database connection

void db_destroy() {
	mysql_close(conn);
}

// Queries all scores from a playlist, indexed by a shortcut id (returns number of scores or -1 on error)
int db_query(char ***scores, int idshortcut) {
	int nrows, i;
	char query[160];
	MYSQL_RES *result;

	sprintf(query, "SELECT source FROM shortcut, score WHERE idshortcut = %d AND shortcut.playlist = score.playlist", idshortcut);

	if (mysql_query(conn, query))
		return -1;

	result = mysql_store_result(conn);

	if (result == NULL)
		return -1;

	nrows = mysql_num_rows(result);
	*scores = (char **)malloc(nrows * sizeof(char*));

	for (i = 0; i < nrows; i++) {
		MYSQL_ROW row = mysql_fetch_row(result);

		(*scores)[i] = (char *)malloc(strlen(row[0]) + strlen(SCORE_HOME) + 1);
		strcpy((*scores)[i], SCORE_HOME);
		strcat((*scores)[i], row[0]);
	}

	return nrows;
}
