#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"

bool CreateDB(const char *create_sentence, const char *db_path) {
	sqlite3 *db;
	int db_ret;
	char *error;
	bool result;

	if (!db_path)
		result = false;
	else {
		db_ret = sqlite3_open(db_path, &db);
		if (db_ret) {
			fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
			result = false;
		}
		else {
			db_ret = sqlite3_exec(db, create_sentence, NULL, 0, &error);
			if (db_ret != SQLITE_OK){
				fprintf(stderr, "sqlite3_exec() error: %s\n", error);
				sqlite3_free(error);
				result = false;
			}	
			else
				result = true;
		}
		sqlite3_close(db);			
	}
	return result;	
}

bool ModifyDB(const char *db_path, const char *modify_sentence) {
	bool result;
	result = CreateDB(modify_sentence, db_path);
	return result;
}

bool QueryDB(const char *db_path, const char *query_sentence, const int column_pos, const int row_pos, char *value, const int value_size) {
	sqlite3 *db;
	sqlite3_stmt *stmt;
	int query_sentence_len;
	int row_index;
	char value_buf[256];
	int db_ret;
	bool result;

	if (!db_path || !query_sentence || !value)
		result = false;
	else {
		db_ret = sqlite3_open(db_path, &db);
		if (db_ret) {
			fprintf(stderr, "sqlite3_open() error: %s\n", sqlite3_errmsg(db));
			result = false;
		}
		else {
			query_sentence_len = strlen(query_sentence);
			db_ret = sqlite3_prepare(db, query_sentence, query_sentence_len, &stmt, NULL);
			if (db_ret != SQLITE_OK) {
				fprintf(stderr, "sqlite3_prepare(%d) error: %s\n", db_ret, sqlite3_errmsg(db));
				result = false;
			}
			else {
				db_ret = sqlite3_step(stmt);
				if ((db_ret != SQLITE_OK) && (db_ret != SQLITE_DONE) && (db_ret != SQLITE_ROW)) {
					fprintf(stderr, "sqlite3_step(%d) error: %s\n", db_ret, sqlite3_errmsg(db));
					result = false;
				}
				else if (db_ret == SQLITE_ROW) {
					for (row_index = 0; row_index <= row_pos; row_index++) {
						sprintf(value_buf, "%s", sqlite3_column_text(stmt, column_pos));
						db_ret = sqlite3_step(stmt);
						if (value_size < strlen(value_buf)+1)
							result = false;
						else {
							if (!strcmp(value_buf, "(null)"))
								strcpy(value, "");
							else
								strcpy(value, value_buf);
							result = true;
						}
					}
				}			
			}
			db_ret = sqlite3_finalize(stmt);
			if (db_ret != SQLITE_OK) {
				fprintf(stderr, "sqlite3_finalize(%d) error: %s\n", db_ret, sqlite3_errmsg(db));
				result = false;
			}
		}
		sqlite3_close(db);
	}
	return result;
}

