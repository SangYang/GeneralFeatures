#ifndef SQLITE_SQLITE_WORK_H
#define SQLITE_SQLITE_WORK_H


bool CreateDB(const char *create_sentence, const char *db_path);
bool ModifyDB(const char *db_path, const char *modify_sentence);
bool QueryDB(const char *db_path, const char *query_sentence, const int column_pos, const int row_pos, char *value, const int value_size);


#endif // SQLITE_SQLITE_WORK_H