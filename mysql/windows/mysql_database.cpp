#include "mysql_database.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#define ASSERT  assert
#define ARRAY_LENGTH(a)  (sizeof(a) / sizeof(a[0]))

#pragma comment(lib, "libmysql")


bool Mysql_Init(const char *p_host, const int port, const char *p_user, const char *p_passwd, MYSQL **pp_handle) {
	MYSQL *p_handle = NULL;

	ASSERT(NULL != p_host && NULL != p_user && NULL != p_passwd && NULL != pp_handle);
	p_handle = mysql_init(NULL);
	if (NULL == p_handle) {
		printf("mysql_init() error=%d,%s\n", mysql_errno(p_handle), mysql_error(p_handle));
		return false;
	}
	else {
		p_handle = mysql_real_connect(p_handle, p_host, p_user, p_passwd, NULL, port, NULL, 0);
		if (NULL == p_handle) {
			printf("mysql_real_connect() error=%d,%s\n", mysql_errno(p_handle), mysql_error(p_handle));
			return false;
		}
		else {
			*pp_handle = p_handle;
			return true;			
		}
	}
}

void Mysql_Uninit(const MYSQL *pc_handle) {
	MYSQL *p_handle = (MYSQL *)pc_handle;

	ASSERT(NULL != p_handle);
	mysql_close(p_handle);
}

bool Mysql_HasTable(const MYSQL *pc_handle, const char *p_db_name, const char *p_table_name) {
	MYSQL *p_handle = (MYSQL *)pc_handle;
	MYSQL_RES *p_result = NULL;
	MYSQL_ROW row;
	char sentence[256];
	int query_ret;

	ASSERT(NULL != p_handle && NULL != p_db_name && NULL != p_table_name);
	sprintf(sentence, "USE %s;", p_db_name);
	query_ret = mysql_query(p_handle, sentence);
	if (0 != query_ret) {
		printf("mysql_query() error=%d,%s\n", mysql_errno(p_handle), mysql_error(p_handle));
		return false;
	}
	else {
		sprintf(sentence, "SHOW TABLES LIKE '%s';", p_table_name);
		query_ret = mysql_real_query(p_handle, sentence, strlen(sentence));
		if (0 != query_ret) {
			printf("mysql_query() error=%d,%s\n", mysql_errno(p_handle), mysql_error(p_handle));
			return false;
		}
		else {
			p_result = mysql_store_result(p_handle);
			row = mysql_fetch_row(p_result);
			mysql_free_result(p_result);
			if (NULL == row) 
				return false;
			else 
				return true;
		}
	}
}

bool Mysql_PrintTables(const MYSQL *pc_handle, const char *p_db_name, const char *p_table_name) {
	MYSQL *p_handle = (MYSQL *)pc_handle;
	MYSQL_RES *p_result = NULL;
	MYSQL_ROW row;
	char sentence[256];
	int query_ret;

	ASSERT(NULL != p_handle && NULL != p_db_name && NULL != p_table_name);
	sprintf(sentence, "USE %s;", p_db_name);
	query_ret = mysql_query(p_handle, sentence);
	if (0 != query_ret) {
		printf("mysql_query() error=%d,%s\n", mysql_errno(p_handle), mysql_error(p_handle));
		return false;
	}
	else {
		if (0 == strcmp(p_table_name, ""))
			strcpy(sentence, "SHOW TABLES;");
		else
			sprintf(sentence, "SHOW TABLES LIKE '%s';", p_table_name);
		query_ret = mysql_real_query(p_handle, sentence, strlen(sentence));
		if (0 != query_ret) {
			printf("mysql_query() error=%d,%s\n", mysql_errno(p_handle), mysql_error(p_handle));
			return false;
		}
		else {
			p_result = mysql_store_result(p_handle);
			row = mysql_fetch_row(p_result);
			while (NULL != row) {
				printf("%s\n", row[0]);
				row = mysql_fetch_row(p_result);
			}
			mysql_free_result(p_result);
			return true;
		}
	}
}

bool Mysql_ExecuteFile(const MYSQL *pc_handle, const char *p_file_path) {
	MYSQL *p_handle = (MYSQL *)pc_handle;
	FILE *pf_sql = NULL;
	char *p_find = NULL;
	char line[256];
	char sentence[1024];
	int query_ret;
	bool ok_file;

	ASSERT(NULL != p_handle && NULL != p_file_path);
	pf_sql = fopen(p_file_path, "rb");
	if (NULL == pf_sql) {
		printf("fopen() error! path=%s\n", p_file_path);
		ok_file = false;
	}
	else {
		while (0 == feof(pf_sql) && 0 == ferror(pf_sql)) {
			memset(sentence, 0, sizeof(sentence));
			do {
				fgets(line, sizeof(line), pf_sql);
				if (0 != ferror(pf_sql) || 0 != feof(pf_sql)) {
					break;
				}
				else {
					sprintf(sentence, "%s%s", sentence, line);
					p_find = strchr(sentence, ';');
					continue;
				}
			}
			while (NULL == p_find);

			if (0 == feof(pf_sql) && 0 == ferror(pf_sql)) {
				query_ret = mysql_real_query(p_handle, sentence, strlen(sentence));
				if (0 != query_ret) {
					printf("mysql_query() error=%d,%s\n", mysql_errno(p_handle), mysql_error(p_handle));
					ok_file = false;
					break;
				}
				else {
					ok_file = true;
					continue;
				}
			}
		}
		fclose(pf_sql);
	}

	return ok_file;
}

// 2016-05-16 09:35:28 to 1447443200
static void GetUnixTime(const char *p_intime, int *p_outtime) {
	struct tm tm_time;
	time_t i_time;
	char *p_tok = NULL;
	char tok[64];
	char time[64];
	char year[8], month[8], day[8];
	char hour[8], minute[8], second[8];
	int index;

	ASSERT(NULL != p_intime && NULL != p_outtime);
	ASSERT(strlen(p_intime) < sizeof(time));
	strcpy(time, p_intime);
	index = 0;
	p_tok = strtok(time, "- :");
	while (NULL != p_tok) {
		switch (index) {
			case 0: 
				tm_time.tm_year = atoi(p_tok) - 1900;
				break;
			case 1: 				
				tm_time.tm_mon = atoi(p_tok) - 1;
				break;
			case 2: 				
				tm_time.tm_mday = atoi(p_tok);
				break;
			case 3: 				
				tm_time.tm_hour = atoi(p_tok);
				break;
			case 4: 			
				tm_time.tm_min = atoi(p_tok);
				break;
			case 5: 				
				tm_time.tm_sec = atoi(p_tok);
				break;
			default: 
				break;
		}
		index++;
		p_tok = strtok(NULL, "- :");
	}
	i_time = mktime(&tm_time);
	*p_outtime = i_time;
}

bool Mysql_ReadEvent(const MYSQL *pc_handle, const int page_index, const int num_perpage, const char *p_table_name, t_EventGroup *p_event_group) {
	t_Event *p_event = NULL;
	t_Jpg *p_jpg = NULL;
	t_CarPlate *p_carplate = NULL;
	t_CarBody *p_carbody = NULL;
	MYSQL *p_handle = (MYSQL *)pc_handle;
	MYSQL_RES *p_result = NULL;
	MYSQL_ROW row;
	char sentence[1024];
	char table[64];
	int start_point;
	int end_point;
	int query_ret;
	int event_index;
	int jpg_index;

	ASSERT(NULL != p_handle && NULL != p_event_group);
	mysql_query(p_handle, "SET NAMES GBK;");
	query_ret = mysql_query(p_handle, "USE car_law_system;");
	if (0 != query_ret) {
		printf("mysql_query() error=%d,%s\n", mysql_errno(p_handle), mysql_error(p_handle));
		return false;
	}
	else {
		start_point = num_perpage * page_index;
		end_point = start_point + num_perpage;
		strcpy(table, p_table_name);
		sprintf(sentence, 
			"SELECT "
			"%s.evt_status,"
			"%s.evt_time,"
			"%s.evt_location,"
			"%s.evt_lane_id,"
			"%s.evt_drive_dir,"
			"%s.evt_drive_dirext,"
			"%s.img_wz_0,"
			"%s.img_wz_1,"
			"%s.img_wz_2,"
			"%s.img_plate,"
			"%s.car_plate,"
			"%s.car_plate_color,"
			"%s.car_plate_confidence,"
			"%s.car_type,"
			"%s.car_color,"
			"%s.car_logo,"
			"%s.car_logo_confidence,"
			"%s.car_chexi,"
			"%s.car_chexi_confidence,"
			"%s.car_chexing,"
			"%s.car_chexing_confidence,"
			"%s.upload_time "
			"FROM "
			"%s "
			"WHERE "
			"%s.evt_time BETWEEN FROM_UNIXTIME(0) AND FROM_UNIXTIME(2147443200) "
			"LIMIT %d, %d;",
			table, table, table, table, table,
			table, table, table, table, table,
			table, table, table, table, table,
			table, table, table, table, table,
			table, table, table, table,
			start_point, end_point);
		query_ret = mysql_query(p_handle, sentence);
		if (0 != query_ret) {
			printf("mysql_query() error=%d,%s\n", mysql_errno(p_handle), mysql_error(p_handle));
			return false;
		}
		else {
			event_index = 0;
			p_result = mysql_store_result(p_handle);
			row = mysql_fetch_row(p_result);
			while (NULL != row) {
				p_event = &p_event_group->m_event[event_index];
				memset(p_event, 0, sizeof(t_Event));
				if (0 == strcmp(p_table_name, "t_violation") || NULL != row[0]) {
					p_event->m_status = atoi(row[0]);
				}
				else {
					p_event->m_status = 0;
				}
				GetUnixTime(row[1], &p_event->m_time);
				strcpy(p_event->m_location, row[2]);
				p_event->m_lane_id = atoi(row[3]);
				p_event->m_drive_dir = atoi(row[4]);
				p_event->m_drive_dirext = atoi(row[5]);
				p_event->m_jpg_count = 0;
				for (jpg_index = 0; jpg_index < ARRAY_LENGTH(p_event->m_jpg); jpg_index++) {
					p_jpg = &p_event->m_jpg[jpg_index];
					strcpy(p_jpg->m_path, row[6 + jpg_index]);
					if (0 != strcmp(row[6 + jpg_index], "")) {
						p_event->m_jpg_count++;
					}
				}
				p_carplate = &p_event->m_car.m_plate;
				p_carbody = &p_event->m_car.m_body;
				strcpy(p_carplate->m_number, row[10]);
				p_carplate->m_color_code = atoi(row[11]);
				p_carplate->m_confidence = atoi(row[12]);
				p_carbody->m_type_code = atoi(row[13]);
				p_carbody->m_color_code = atoi(row[14]);
				p_carbody->m_logo_code = atoi(row[15]);
				p_carbody->m_logo_confidence = atoi(row[16]);
				p_carbody->m_chexi_code = atoi(row[17]);
				p_carbody->m_chexi_confidence = atoi(row[18]);
				p_carbody->m_chexing_code = atoi(row[19]);
				p_carbody->m_chexing_confidence = atoi(row[20]);
				if (0 == strcmp(p_table_name, "t_violation") || NULL != row[21]) {
					GetUnixTime(row[21], &p_event->m_upload_time);
				}
				else {
					p_event->m_upload_time = 0;
				}
				//printf("%s %s %s %s %s\n", row[0], row[1], row[2], row[3], row[9]);
				event_index++;
				row = mysql_fetch_row(p_result);
			}
			mysql_free_result(p_result);	

			p_event_group->m_event_count = event_index;
			return true;
		}	
	}
}


static void ModifyPath(const char *p_path, char *p_modPath, const int modSize) {
	char *p_tok = NULL;
	char tok[256] = {0};

	ASSERT(NULL != p_path && NULL != p_modPath);
	ASSERT(modSize > strlen(p_path));
	strcpy(p_modPath, p_path);
	p_tok = strtok(p_modPath, "\\");
	while (NULL != p_tok) {
		strcat(tok, p_tok);
		strcat(tok, "\\\\");
		p_tok = strtok(NULL, "\\");
	}
	ASSERT(modSize > strlen(tok));
	strncpy(p_modPath, tok, strlen(tok) - 2);
	p_modPath[strlen(tok) - 2] = '\0'; 
}


bool Mysql_WriteEvent(const MYSQL *pc_handle, const t_EventGroup *p_event_group, const char *p_table_name) {
	const t_Event *p_event = NULL;
	const t_Jpg *p_jpg = NULL;
	const t_CarPlate *p_carplate = NULL;
	const t_CarBody *p_carbody = NULL;
	MYSQL *p_handle = (MYSQL *)pc_handle;
	MYSQL_RES *p_result = NULL;
	MYSQL_ROW row;
	char sentence[1024];
	char jpg_path[256];
	int query_ret;
	int event_index;
	int jpg_index;


	ASSERT(NULL != p_handle && NULL != p_event_group);
	mysql_query(p_handle, "SET NAMES GBK;");
	query_ret = mysql_query(p_handle, "USE car_law_system;");
	if (0 != query_ret) {
		printf("mysql_query() error=%d,%s\n", mysql_errno(p_handle), mysql_error(p_handle));
		return false;
	}
	else {
		for (event_index = 0; event_index < p_event_group->m_event_count; event_index++) {
			p_event = &p_event_group->m_event[event_index];
			if (0 == strcmp(p_table_name, "t_event")) {
				sprintf(sentence, "INSERT INTO `%s` VALUES (NULL, 0", p_table_name);	
			}
			else if (0 == strcmp(p_table_name, "t_violation")) {
				sprintf(sentence, "INSERT INTO `%s` VALUES (NULL, %d", p_table_name, p_event->m_status);
			}
			else {
				printf("Mysql_WriteEvent() error! Input event_type is error!\n");
				return false;
			}
			sprintf(sentence, "%s, FROM_UNIXTIME(%d), '%s', %d, %d, %d", 
				sentence, p_event->m_time, p_event->m_location, 
				p_event->m_lane_id, p_event->m_drive_dir, p_event->m_drive_dirext
				);
			for (jpg_index = 0; jpg_index < ARRAY_LENGTH(p_event->m_jpg); jpg_index++) {
				if (jpg_index < p_event->m_jpg_count) {
					p_jpg = &p_event->m_jpg[jpg_index];
					ModifyPath(p_jpg->m_path, jpg_path, sizeof(jpg_path));
					sprintf(sentence, "%s, '%s'", sentence, jpg_path);
				}
				else
					sprintf(sentence, "%s, ''", sentence);
			}
			p_carplate = &p_event->m_car.m_plate;
			p_carbody = &p_event->m_car.m_body;
			sprintf(sentence, "%s, '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d", 
				sentence, p_carplate->m_number, p_carplate->m_color_code, p_carplate->m_confidence,
				p_carbody->m_type_code, p_carbody->m_color_code, p_carbody->m_logo_code, p_carbody->m_logo_confidence, 
				p_carbody->m_chexi_code, p_carbody->m_chexi_confidence, p_carbody->m_chexing_code, p_carbody->m_chexing_confidence
				);

			if (0 == strcmp(p_table_name, "t_event"))
				sprintf(sentence, "%s, NULL", sentence);
			else if (0 == strcmp(p_table_name, "t_violation")) 
				sprintf(sentence, "%s, FROM_UNIXTIME(%d)", sentence, p_event->m_upload_time);
			else {
				printf("Mysql_WriteEvent() error! Input event_type is error!\n");
				return false;
			}
			sprintf(sentence, "%s, FROM_UNIXTIME(%d));", sentence, (int)time(NULL));

			query_ret = mysql_query(p_handle, sentence);
			if (0 != query_ret) {
				printf("mysql_query() error=%d,%s\n", mysql_errno(p_handle), mysql_error(p_handle));
				return false;
			}
		}
		return true;	
	}	
}

bool Mysql_ReadCategory(const MYSQL *pc_handle,  t_EventGroup *p_event_group) {
/*
	t_Event *p_event = NULL;
	t_Jpg *p_jpg = NULL;
	t_CarPlate *p_carplate = NULL;
	t_CarBody *p_carbody = NULL;
	MYSQL *p_handle = (MYSQL *)pc_handle;
	MYSQL_RES *p_result = NULL;
	MYSQL_ROW row;
	char sentence[1024];
	char table[64];
	int start_point;
	int end_point;
	int query_ret;
	int event_index;
	int jpg_index;

	ASSERT(NULL != p_handle && NULL != p_event_group);
	mysql_query(p_handle, "SET NAMES GBK;");
	query_ret = mysql_query(p_handle, "USE car_law_system;");
	if (0 != query_ret) {
		printf("mysql_query() error=%d,%s\n", mysql_errno(p_handle), mysql_error(p_handle));
		return false;
	}
	else {
		start_point = num_perpage * page_index;
		end_point = start_point + num_perpage;
		strcpy(table, p_table_name);
		sprintf(sentence, 
			"SELECT "
			"%s.evt_status,"
			"%s.evt_time,"
			"%s.evt_location,"
			"%s.evt_lane_id,"
			"%s.evt_drive_dir,"
			"%s.evt_drive_dirext,"
			"%s.img_wz_0,"
			"%s.img_wz_1,"
			"%s.img_wz_2,"
			"%s.img_plate,"
			"%s.car_plate,"
			"%s.car_plate_color,"
			"%s.car_plate_confidence,"
			"%s.car_type,"
			"%s.car_color,"
			"%s.car_logo,"
			"%s.car_logo_confidence,"
			"%s.car_chexi,"
			"%s.car_chexi_confidence,"
			"%s.car_chexing,"
			"%s.car_chexing_confidence,"
			"%s.upload_time "
			"FROM "
			"%s "
			"WHERE "
			"%s.evt_time BETWEEN FROM_UNIXTIME(0) AND FROM_UNIXTIME(2147443200) "
			"LIMIT %d, %d;",
			table, table, table, table, table,
			table, table, table, table, table,
			table, table, table, table, table,
			table, table, table, table, table,
			table, table, table, table,
			start_point, end_point);
		query_ret = mysql_query(p_handle, sentence);
		if (0 != query_ret) {
			printf("mysql_query() error=%d,%s\n", mysql_errno(p_handle), mysql_error(p_handle));
			return false;
		}
		else {
			event_index = 0;
			p_result = mysql_store_result(p_handle);
			row = mysql_fetch_row(p_result);
			while (NULL != row) {
				p_event = &p_event_group->m_event[event_index];
				memset(p_event, 0, sizeof(t_Event));
				if (0 == strcmp(p_table_name, "t_violation") || NULL != row[0]) {
					p_event->m_status = atoi(row[0]);
				}
				else {
					p_event->m_status = 0;
				}
				GetUnixTime(row[1], &p_event->m_time);
				strcpy(p_event->m_location, row[2]);
				p_event->m_lane_id = atoi(row[3]);
				p_event->m_drive_dir = atoi(row[4]);
				p_event->m_drive_dirext = atoi(row[5]);
				p_event->m_jpg_count = 0;
				for (jpg_index = 0; jpg_index < ARRAY_LENGTH(p_event->m_jpg); jpg_index++) {
					p_jpg = &p_event->m_jpg[jpg_index];
					strcpy(p_jpg->m_path, row[6 + jpg_index]);
					if (0 != strcmp(row[6 + jpg_index], "")) {
						p_event->m_jpg_count++;
					}
				}
				p_carplate = &p_event->m_car.m_plate;
				p_carbody = &p_event->m_car.m_body;
				strcpy(p_carplate->m_number, row[10]);
				p_carplate->m_color_code = atoi(row[11]);
				p_carplate->m_confidence = atoi(row[12]);
				p_carbody->m_type_code = atoi(row[13]);
				p_carbody->m_color_code = atoi(row[14]);
				p_carbody->m_logo_code = atoi(row[15]);
				p_carbody->m_logo_confidence = atoi(row[16]);
				p_carbody->m_chexi_code = atoi(row[17]);
				p_carbody->m_chexi_confidence = atoi(row[18]);
				p_carbody->m_chexing_code = atoi(row[19]);
				p_carbody->m_chexing_confidence = atoi(row[20]);
				if (0 == strcmp(p_table_name, "t_violation") || NULL != row[21]) {
					GetUnixTime(row[21], &p_event->m_upload_time);
				}
				else {
					p_event->m_upload_time = 0;
				}
				//printf("%s %s %s %s %s\n", row[0], row[1], row[2], row[3], row[9]);
				event_index++;
				row = mysql_fetch_row(p_result);
			}
			mysql_free_result(p_result);	

			p_event_group->m_event_count = event_index;
			return true;
		}	
	}*/
return true;
}




