#include <stdio.h>
#include "sqlite_work.h"

int main() {
	const char db_path[] = "my_2.db";
	const char create_sentence[] = 
		"CREATE TABLE Cars(Id integer PRIMARY KEY, Name text, Cost integer);"
		"INSERT INTO Cars VALUES(1,'Audi',52642);"
		"INSERT INTO Cars VALUES(2,'Mercedes',57127);"
		"INSERT INTO Cars VALUES(3,'Skoda',9000);"
		"INSERT INTO Cars VALUES(4,'Volvo',29000);"
		"INSERT INTO Cars VALUES(5,'Bentley',350000);"
		"INSERT INTO Cars VALUES(6,'Citroen',21000);"
		"INSERT INTO Cars VALUES(7,'Hummer',41400);"
		"INSERT INTO Cars VALUES(8,'Volkswagen',21600);";
	const char modify_sentence[] = 
		"UPDATE Cars SET Name='abcde' WHERE Id=5;"
		"UPDATE Cars SET Cost='12345' WHERE Id=5;"
		"INSERT INTO Cars VALUES(100,'China',123456789);"
		"DELETE FROM Cars WHERE Cost<10000;"
		"INSERT INTO Cars(Name,Cost) VALUES('BWM',222222);"
		"INSERT INTO Cars(Name) VALUES('DAZHONG');";
	char query_value_1[32], query_value_2[32], query_value_3[32], query_value_4[32];
	CreateDB(create_sentence, db_path);
	ModifyDB(db_path, modify_sentence);
	QueryDB(db_path, "SELECT * FROM Cars WHERE Name Like '_______';", 2, 1, query_value_1, 256);
	QueryDB(db_path, "SELECT * FROM Cars WHERE Name Like '%en';", 1, 1, query_value_2, 256);
	QueryDB(db_path, "SELECT * FROM Cars WHERE Cost BETWEEN 20000 AND 55000;", 2, 3, query_value_3, 256);
	QueryDB(db_path, "SELECT * FROM Cars WHERE Name IN ('Audi','Hummer');", 1, 1, query_value_4, 256);

	return 0;
}
