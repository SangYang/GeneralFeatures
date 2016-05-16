#ifndef WINDOWS_SOCKET_H
#define WINDOWS_SOCKET_H

#include <winsock2.h>

typedef struct {
	SOCKET m_handle;
	char   m_ip[32];
	int    m_port;
} t_Socket;

void g_GetSocket(t_Socket *p_socket);
void g_SetSocket(const t_Socket socket);

bool InitSocket(SOCKET *p_handle);
bool SetSocketServer(const char *p_ip, const int port, const SOCKET listen_handle);
bool WaitConnectSocket(const SOCKET listen_handle, SOCKET *p_accept_handle);
bool ConnectSocket(const char *p_ip, const int port, const SOCKET client_handle);
int  SendSocket(const SOCKET handle, const void *pv_data, const int size);
int  RecvSocket(const SOCKET handle, void *pv_data, const int size);
void UninitSocket(const SOCKET handle);


#endif // WINDOWS_SOCKET_H

