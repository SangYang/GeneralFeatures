#include <stdio.h>
#include <string.h>
#include <time.h>
#include "basic_data.h"
#include "mysql_database.h"


#if 0
int main(void) {
	CRTMEM_BEGIN();
	MYSQL *p_handle;
	bool ok_init;
	t_EventGroup event_group;

	ok_init = Mysql_Init("192.168.1.66", 3306, "ssyang", "ssyang", &p_handle);
	if (false == ok_init) {
		printf("Mysql_Init() error!\n");
	}
	else {
		InitEventGroup(&event_group);
		
		bool write = Mysql_WriteEvent(p_handle, &event_group, "t_violation");

		//Mysql_PrintTables(p_handle, "car_law_system", "");
		bool read = Mysql_ReadEvent(p_handle, 0, 100, "t_violation", &event_group);
		PrintEventGroup(&event_group);

		Mysql_Uninit(p_handle);
	}


	CRTMEM_END();
	return 0;
}
#endif

#if 0
int main(void) {
	CRTMEM_BEGIN();

	MYSQL *p_handle;
	Mysql_Init("192.168.1.66", 3306, "ssyang", "ssyang", &p_handle);
	Mysql_PrintTables(p_handle, "blog", "%_%");
	bool abc = Mysql_HasTable(p_handle, "blog", "ad");
	bool abcd = Mysql_HasTable(p_handle, "blog", "blog_article");
	bool aaa = Mysql_ExecuteFile(p_handle, "E:\\2_OpenSourceRroject\\mysql\\newdb.sql");
	Mysql_Uninit(p_handle);

	CRTMEM_END();
	return 0;
}
#endif


#if 0
int main(void) {  
	CRTMEM_BEGIN();

	MYSQL *p_handle1;
	Mysql_Init("192.168.1.66", 3306, "ssyang", "ssyang", &p_handle1);
	Mysql_PrintTables(p_handle1, "blog", "%_ar%");
	bool abc = Mysql_HasTable(p_handle1, "blog", "ad");
	bool abcd = Mysql_HasTable(p_handle1, "blog", "blog_article");
	bool aaa = Mysql_ExecuteFile(p_handle1, "E:\\2_OpenSourceRroject\\mysql\\newdb.sql");
	Mysql_Uninit(p_handle1);


	MYSQL *p_handle; 
	MYSQL_RES *p_mysql_res;  
	MYSQL_ROW mysql_row; 
	int query_ret;

	p_handle = mysql_init(NULL);  
	if (NULL == p_handle) {
		printf("mysql_init() error=%d,%s\n", mysql_errno(p_handle), mysql_error(p_handle));
	}
	else 
	{
		p_handle = mysql_real_connect(p_handle, "192.168.1.66", "ssyang", "ssyang", NULL/*"t_event"*/, 3306, NULL, CLIENT_MULTI_STATEMENTS);
		if (NULL == p_handle) {
			printf("mysql_real_connect() error=%d,%s\n", mysql_errno(p_handle), mysql_error(p_handle));
		}
		else {
			//query_ret = mysql_query(p_handle, "use blog;");
			query_ret = mysql_real_query(p_handle, /*"select * from blog_user;"*/"use blog; show tables;",(unsigned int)strlen("use blog; show tables;"));
			if (0 != query_ret) {
				printf("mysql_query() error=%d,%s\n", mysql_errno(p_handle), mysql_error(p_handle));
			}
			else {
				do {
					p_mysql_res = mysql_store_result(p_handle);
					if (NULL == p_mysql_res) {
						printf("eeee\n");
					}
					else {
						mysql_row = mysql_fetch_row(p_mysql_res);
						while (NULL != mysql_row) {
							printf("%s\n", mysql_row[0]);
							mysql_row = mysql_fetch_row(p_mysql_res);
						}
						mysql_free_result(p_mysql_res); 
					}
				}
				while (!mysql_next_result( p_handle ) );


				//mysql_free_result(p_mysql_res);
			}
		}
		mysql_close(p_handle);
	}

	CRTMEM_END();
	return 0;
}  
#endif
