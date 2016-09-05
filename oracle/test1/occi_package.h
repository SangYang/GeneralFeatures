#ifndef SSY_OCCI_PACKAGE_H
#define SSY_OCCI_PACKAGE_H


typedef struct {
	char m_first[64];
	char m_second[64];
	char m_three[64];
} t_OcciResult;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

	void* OCCI_InitEnvironment(const char *charset, const char *ncharset);
	void OCCI_UninitEnvironment(const void *environment);
	void* OCCI_CreateConnectionPool(const void *environment, const char *username, const char *password, const char *connectString, int maxConn);
	void OCCI_DestroyConnectionPool(const void *environment, const void *connPool);
	void* OCCI_CreateConnectionByPool(const void *connPool, const char *username, const char *password);
	void OCCI_DestroyConnectionByPool(const void *connPool, const void *conn);
	void* OCCI_CreateConnection(const void *environment, const char *username, const char *password, const char *connectString);
	void OCCI_DestroyConnection(const void *environment, const void *conn);
	void* OCCI_CreateStatement(const void *connection);
	void OCCI_DestroyStatement(const void *connection, const void *statement);
	void OCCI_QuerySql(const void *statement, void (*OCCI_DoQueryResult)(t_OcciResult *occiResult));
	int OCCI_UpdateSql(const void *statement); // 1:success  !1:failure
	int OCCI_UpdateMultiSql(const void *connection, const void *statement);

#ifdef __cplusplus
};
#endif // __cplusplus

/*
 * select * from nls_database_parameters // 服务器字符集环境
 * select * from nls_instance_parameters // 客户端字符集环境
 * select * from nls_session_parameters  // 会话字符集环境
 */


#endif // SSY_OCCI_PACKAGE_H

