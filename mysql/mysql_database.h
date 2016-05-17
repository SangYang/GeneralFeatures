#ifndef SSY_MYSQL_DATABASE_H
#define SSY_MYSQL_DATABASE_H


#include <WinSock.h>
#include "mysql.h"
#include "basic_data.h"

bool Mysql_Init(const char *p_host, const int port, const char *p_user, const char *p_passwd, MYSQL **pp_handle);
void Mysql_Uninit(const MYSQL *pc_handle);
bool Mysql_HasTable(const MYSQL *pc_handle, const char *p_db_name, const char *p_table_name);
bool Mysql_PrintTables(const MYSQL *pc_handle, const char *p_db_name, const char *p_table_name);
bool Mysql_ExecuteFile(const MYSQL *pc_handle, const char *p_file_path);
bool Mysql_ReadEvent(const MYSQL *pc_handle, const int page_index, const int num_perpage, const char *p_table_name, t_EventGroup *p_event_group);
bool Mysql_WriteEvent(const MYSQL *pc_handle, const t_EventGroup *p_event_group, const char *p_table_name);


#endif // SSY_MYSQL_DATABASE_H