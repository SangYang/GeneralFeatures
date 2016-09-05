#include <iostream>
#include <string>
#include "occi.h"

#pragma comment(lib, "oraocci11")

using namespace std;
using namespace oracle::occi;

int main(void) {
	Environment *env;
	Connection *conn;
	Statement *stmt;
	ResultSet *rs;
	string username = "scott";
	string password = "tiger";
	string connstring = "192.168.1.66:1521/orcl";
	string sql;

	env = Environment::createEnvironment(Environment::DEFAULT);
	conn = env->createConnection(username, password, connstring);
	stmt = conn->createStatement();

	sql = "SELECT "
		"SCOTT.EMP.EMPNO, "
		"SCOTT.EMP.ENAME, "
		"SCOTT.EMP.JOB, "
		"SCOTT.EMP.MGR, "
		"SCOTT.EMP.HIREDATE, "
		"SCOTT.EMP.SAL, "
		"SCOTT.EMP.COMM, "
		"SCOTT.EMP.DEPTNO "
		"FROM "
		"SCOTT.EMP";
	printf("%s\n", sql.c_str());
	stmt->setSQL(sql);
	try {
		rs = stmt->executeQuery();
		while (rs->next()) {
			printf("%s	%s	%s\n", rs->getString(1).c_str(), rs->getString(2).c_str(), rs->getString(3).c_str());           
		}
	}
	catch (SQLException &ex) {
		printf("[Error:%d] %s\n", ex.getErrorCode(), ex.getMessage().c_str());
	}

	conn->terminateStatement(stmt);
	env->terminateConnection(conn);
	Environment::terminateEnvironment(env);

	return 0;
}