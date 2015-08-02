/*
 * Database access
 * Victor Manuel Fernandez Castro
 * 2 August 2015
 */

#include "database.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <mysql.h>

static const char DB_HOST[] = "localhost";
static const char DB_USER[] = "organo";
static const char DB_PASS[] = "ugr2015";
static const char DB_NAME[] = "organo";

static MYSQL *conn;

// Initializes the database. Returns 0 on success or -1 on error

int database_init() {
	conn = mysql_init(NULL);
	
	if (conn == NULL)
		return -1;
	
	if (!mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 0, NULL, 0))
		return -1;
	
	return 0;
}

// Close a database connection

void database_destroy() {
	mysql_close(conn);
}

// Queries all scores from a playlist, returns number of scores or -1 on error

int database_query(score_t **scores, int idplaylist) {
	int nrows, i;
	char query[80];
	MYSQL_RES *result;

	sprintf(query, "SELECT idscore, source FROM score WHERE playlist = %d ORDER BY sorting ASC", idplaylist);

	if (mysql_query(conn, query))
		return -1;
	
	result = mysql_store_result(conn);

	if (result == NULL)
		return -1;
	
	nrows = mysql_num_rows(result);
	*scores = (score_t *)malloc(nrows * sizeof(score_t));
	
	if (*scores == NULL)
		return -1;
	
	for (i = 0; i < nrows; i++) {
		score_t *score = (*scores) + i;
		MYSQL_ROW row = mysql_fetch_row(result);
		score->idscore = atoi(row[0]);
		score->source = (char *)malloc(strlen(row[1]));
		strcpy(score->source, row[1]);
	}
	
	return nrows;
}

// Delete an array of scores

void score_destroy(score_t *scores, int n) {
	int i;
	
	for (i = 0; i < n; i++)
		free(scores[i].source);
	
	free(scores);
}
