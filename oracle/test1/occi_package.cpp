#include "occi_package.h"
#include <iostream>
#include <string>
#include <ctime>
#include "occi.h"
#include "common_log.h"

#pragma comment(lib, "oraocci11")

using namespace std;
using namespace oracle::occi;

void* OCCI_InitEnvironment(const char *charset, const char *ncharset) {
	try {
		Environment *environment = Environment::createEnvironment(Environment::DEFAULT);//(charset, ncharset);//("ZHS16GBK", "ZHS16GBK", Environment::DEFAULT);
		return environment;	
	}
	catch (SQLException &ex) {
		LOGERROR("[OCCI]createEnvironment(%s,%s)=NULL\n", charset, ncharset);
		LOGERROR("[OCCI]%d:%s\n", ex.getErrorCode(), ex.getMessage().c_str());
	}
}

void OCCI_UninitEnvironment(const void *environment) {
	try {
		if (NULL != environment) 
			Environment::terminateEnvironment((Environment *)environment);
	}
	catch (SQLException &ex) {
		LOGERROR("[OCCI]terminateEnvironment(0x%p)=void\n", environment);
		LOGERROR("[OCCI]%d:%s\n", ex.getErrorCode(), ex.getMessage().c_str());
	}
}

void* OCCI_CreateConnectionPool(const void *environment, const char *username, const char *password, const char *connectString, int maxConn) {
	try {
		ConnectionPool *connPool = NULL;
		if (NULL != environment) 
			connPool = ((Environment *)environment)->createConnectionPool(username, password, connectString, 0, maxConn, 1);		
		return connPool;	
	}
	catch (SQLException &ex) {
		LOGERROR("[OCCI]createConnectionPool(%s,%s,%s,%d)=NULL\n", username, password, connectString, maxConn);
		LOGERROR("[OCCI]%d:%s\n", ex.getErrorCode(), ex.getMessage().c_str());
	}
}

void OCCI_DestroyConnectionPool(const void *environment, const void *connPool) {
	try {
		if (NULL != environment && NULL != connPool) 
			((Environment *)environment)->terminateConnectionPool((ConnectionPool *)connPool);
	}
	catch (SQLException &ex) {
		LOGERROR("[OCCI]0x%p->terminateConnectionPool(0x%p)=void\n", environment, connPool);
		LOGERROR("[OCCI]%d:%s\n", ex.getErrorCode(), ex.getMessage().c_str());
	}
}

void* OCCI_CreateConnectionByPool(const void *connPool, const char *username, const char *password) {
	try {
		Connection *conn = NULL;
		if (NULL != connPool) 
			conn = ((ConnectionPool *)connPool)->createConnection(username, password);	
		return conn;
	}
	catch (SQLException &ex) {
		LOGERROR("[OCCI]createConnection(%s,%s)=NULL\n", username, password);
		LOGERROR("[OCCI]%d:%s\n", ex.getErrorCode(), ex.getMessage().c_str());
	}
}

void OCCI_DestroyConnectionByPool(const void *connPool, const void *conn) {
	try {
		if (NULL != connPool && NULL != conn) 
			((ConnectionPool *)connPool)->terminateConnection((Connection *)conn);	
	}
	catch (SQLException &ex) {
		LOGERROR("[OCCI]0x%p->terminateConnection(0x%p)=void\n", connPool, conn);
		LOGERROR("[OCCI]%d:%s\n", ex.getErrorCode(), ex.getMessage().c_str());
	}
}

void* OCCI_CreateConnection(const void *environment, const char *username, const char *password, const char *connectString) {
	try {
		Connection *conn = NULL;
		if (NULL != environment) 
			conn = ((Environment *)environment)->createConnection(username, password, connectString);	
		return conn;
	}
	catch (SQLException &ex) {
		LOGERROR("[OCCI]createConnection(%s,%s,%s)=NULL\n", username, password, connectString);
		LOGERROR("[OCCI]%d:%s\n", ex.getErrorCode(), ex.getMessage().c_str());
	}
}

void OCCI_DestroyConnection(const void *environment, const void *conn) {
	try {
		if (NULL != environment && NULL != conn) 
			((Environment *)environment)->terminateConnection((Connection *)conn);	
	}
	catch (SQLException &ex) {
		LOGERROR("[OCCI]0x%p->terminateConnection(0x%p)=void\n", environment, conn);
		LOGERROR("[OCCI]%d:%s\n", ex.getErrorCode(), ex.getMessage().c_str());
	}
}

void* OCCI_CreateStatement(const void *connection) {
	try {
		Statement *statement = NULL;
		if (NULL != connection) 
			statement = ((Connection *)connection)->createStatement();	
		return statement;
	}
	catch (SQLException &ex) {
		LOGERROR("[OCCI]createStatement()=NULL\n");
		LOGERROR("[OCCI]%d:%s\n", ex.getErrorCode(), ex.getMessage().c_str());
	}
}

void OCCI_DestroyStatement(const void *connection, const void *statement) {
	try {
		if (NULL != connection && NULL != statement) 
			((Connection *)connection)->terminateStatement((Statement *)statement);
	}
	catch (SQLException &ex) {
		LOGERROR("[OCCI]0x%p->terminateStatement(0x%p)=void\n", connection, statement);
		LOGERROR("[OCCI]%d:%s\n", ex.getErrorCode(), ex.getMessage().c_str());
	}
}

void OCCI_QuerySql(const void *statement, void (*OCCI_DoQueryResult)(t_OcciResult *occiResult)) {
	t_OcciResult occiResult;
	Statement *stmt = (Statement *)statement;
	ResultSet *resultSet;
	char sql[] = "SELECT "
		"SCOTT.EMP.EMPNO, "
		"SCOTT.EMP.ENAME, "
		"SCOTT.EMP.JOB, "
		"SCOTT.EMP.MGR, "
		"SCOTT.EMP.HIREDATE, "
		"SCOTT.EMP.SAL, "
		"SCOTT.EMP.COMM, "
		"SCOTT.EMP.DEPTNO "
		"FROM "
		"SCOTT.EMP "
		"WHERE "
		"ROWNUM <= 30 AND "
		"SCOTT.EMP.HIREDATE >= TO_DATE('1980-01-01 00:00:00', 'YYYY-MM-DD HH24:MI:SS') AND "
		"SCOTT.EMP.HIREDATE < TO_DATE('1982-01-01 00:00:00', 'YYYY-MM-DD HH24:MI:SS') AND "
		"SCOTT.EMP.JOB IN ('MANAGER', 'ANALYST', 'BBBB') "
		"ORDER BY "
		"SCOTT.EMP.EMPNO ASC"; // ASC:ÉýÐò DESC:½µÐò

	LOGINFO("%s\n", sql);
	try {
		stmt->setSQL(sql);
		resultSet = stmt->executeQuery();
		while (resultSet->next()) {
			memset(&occiResult, 0, sizeof(t_OcciResult));           
			strcpy(occiResult.m_first, resultSet->getString(1).c_str());
			strcpy(occiResult.m_second, resultSet->getString(2).c_str());
			strcpy(occiResult.m_three, resultSet->getString(3).c_str());			
			OCCI_DoQueryResult(&occiResult);  
		}
		stmt->closeResultSet(resultSet);
	}
	catch (SQLException &ex) {
		LOGERROR("[OCCI]statement=0x%p, sql=%s\n", statement, sql);
		LOGERROR("[OCCI]%d:%s\n", ex.getErrorCode(), ex.getMessage().c_str());
	}
}

int OCCI_UpdateSql(const void *statement) {
	Statement *stmt = (Statement *)statement;
	int updateRet;
	char sql[] = "INSERT INTO "
		"SCOTT.EMP ("	
		"SCOTT.EMP.EMPNO, "
		"SCOTT.EMP.ENAME, "
		"SCOTT.EMP.JOB, "
		"SCOTT.EMP.MGR, "
		"SCOTT.EMP.HIREDATE) "
		"VALUES ("
		"1111, 'AAAA', 'BBBB', 2221, TO_DATE('1980-01-01 00:00:00', 'YYYY-MM-DD HH24:MI:SS'))";
/*
	char sql[] = "UPDATE " 
		"SCOTT.EMP "
		"SET "
		"SCOTT.EMP.ENAME = 'AAAA', " 
		"SCOTT.EMP.JOB = 'BBBB', "
		"SCOTT.EMP.MGR = '1234', "
		"SCOTT.EMP.HIREDATE = TO_DATE('1980-01-01 00:00:00', 'YYYY-MM-DD HH24:MI:SS') "
		"WHERE "
		"SCOTT.EMP.EMPNO = 1111";
*/
	LOGINFO("%s\n", sql);
	try {
		stmt->setSQL(sql);
		updateRet = (int)stmt->executeUpdate();
		return updateRet;
	}
	catch (SQLException &ex) {
		LOGERROR("[OCCI]statement=0x%p, sql=%s\n", statement, sql);
		LOGERROR("[OCCI]%d:%s\n", ex.getErrorCode(), ex.getMessage().c_str());
	}
}

int OCCI_UpdateMultiSql(const void *connection, const void *statement) {
	Connection *conn = (Connection *)connection;
	Statement *stmt = (Statement *)statement;
	int iterMax, iterIdx;
	int updateRet;
	int key[] = {1111, 2222, 3333, 4444};
	char *ename[] = {"A", "AA", "AAA", "AAAA"};
/*
	char sql[] = "INSERT INTO "
		"SCOTT.EMP ("	
		"SCOTT.EMP.ENAME, "
		"SCOTT.EMP.EMPNO, "
		"SCOTT.EMP.JOB, "
		"SCOTT.EMP.MGR, "
		"SCOTT.EMP.HIREDATE) "
		"VALUES ("
		":1, :2, 'BBBB', 2221, TO_DATE('1980-01-01 00:00:00', 'YYYY-MM-DD HH24:MI:SS'))";
*/
	char sql[] = "UPDATE " 
		"SCOTT.EMP "
		"SET "
		"SCOTT.EMP.ENAME = :1, " 
		"SCOTT.EMP.JOB = 'BBBB', "
		"SCOTT.EMP.MGR = '1234', "
		"SCOTT.EMP.HIREDATE = TO_DATE('1980-01-01 00:00:00', 'YYYY-MM-DD HH24:MI:SS') "
		"WHERE "
		"SCOTT.EMP.EMPNO = :2";

	LOGINFO("%s\n", sql);
	iterMax = sizeof(key) / sizeof(key[0]);
	try {
		stmt->setAutoCommit(false);
		stmt->setSQL(sql);
		stmt->setMaxIterations(iterMax); 
		stmt->setMaxParamSize(1, sizeof(string));
		stmt->setMaxParamSize(2, sizeof(int));
		for (iterIdx = 0; iterIdx < iterMax; iterIdx++) {
			stmt->setString(1, ename[iterIdx]);
			stmt->setInt(2, key[iterIdx]);
			if (iterIdx != iterMax-1)
				stmt->addIteration();
		}			
		updateRet = (int)stmt->executeUpdate();
		conn->commit();
		return updateRet;
	}
	catch (SQLException &ex) {
		LOGERROR("[OCCI]connection=0x%p, statement=0x%p, getUpdateCount()=%d, sql=%s\n", connection, statement, stmt->getUpdateCount(), sql);
		LOGERROR("[OCCI]%d:%s\n", ex.getErrorCode(), ex.getMessage().c_str());
		conn->rollback();
	}
}

#if 1
void OCCI_DoQueryResult(t_OcciResult *occiResult) {
	printf("[result]%s-%s-%s\n", occiResult->m_first, occiResult->m_second, occiResult->m_three);
}

int main(void) {
	void *environment = NULL;
	void *connPool = NULL;
	void *connection1 = NULL, *connection2 = NULL, *connection3 = NULL;
	void *statement1 = NULL, *statement2 = NULL, *statement3 = NULL;


	environment = OCCI_InitEnvironment("UTF8", "UTF8");
	//connPool = OCCI_CreateConnectionPool(environment, "scott", "tiger", "192.168.1.66:1521/orcl", 2);
	//connection1 = OCCI_CreateConnectionByPool(connPool, "scott", "tiger");
	//connection2 = OCCI_CreateConnectionByPool(connPool, "scott", "tiger");
	connection1 = OCCI_CreateConnection(environment, "scott", "tiger", "192.168.1.66:1521/orcl");
	connection2 = OCCI_CreateConnection(environment, "scott", "tiger", "192.168.1.66:1521/orcl");
	//while (1) 
	{
		statement1 = OCCI_CreateStatement(connection1);
		statement2 = OCCI_CreateStatement(connection2);

		while (1) {
		int updateRet = OCCI_UpdateMultiSql(connection1, statement1);
		printf("updateRet=%d\n", updateRet);
		}

		OCCI_QuerySql(statement1, OCCI_DoQueryResult);
		printf("\n");
		//OCCI_QuerySql(statement2, OCCI_DoQueryResult);
		//printf("======================\n");

		OCCI_DestroyStatement(connection2, statement2);
		OCCI_DestroyStatement(connection1, statement1);
	}
	OCCI_DestroyConnection(environment, connection2);
	OCCI_DestroyConnection(environment, connection1);
	//OCCI_DestroyConnectionByPool(connPool, connection2);
	//OCCI_DestroyConnectionByPool(connPool, connection1);
	//OCCI_DestroyConnectionPool(environment, connPool);
	OCCI_UninitEnvironment(environment);
	
	return 0;
}
#endif 

