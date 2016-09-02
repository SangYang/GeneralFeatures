#ifndef WINDOWS_SOCKET_H
#define WINDOWS_SOCKET_H


#include <winsock2.h>

#define SOCKE_SUCCESS          (1)
#define SOCK_ERR_WSASTARTUP    -201
#define SOCK_ERR_VERSION       -202
#define SOCK_ERR_SOCKETTCP     -203
#define SOCK_ERR_SETKEEPALIVE  -204
#define SOCK_ERR_SETSNDTIME    -205
#define SOCK_ERR_SETRCVTIME    -206
#define SOCK_ERR_BIND          -207
#define SOCK_ERR_LISTEN        -208
#define SOCK_ERR_ACCEPT        -209
#define SOCK_ERR_CONNECT       -210
#define SOCK_ERR_CONNECTED     -211
#define SOCK_ERR_SEND          -212
#define SOCK_ERR_RECV          -213

#ifdef __cplusplus
extern "C" {
#endif

	int Socket_Init(SOCKET *handle);
	int Socket_SetServer(const char *ip, const int port, const SOCKET listenHandle);
	int Socket_WaitConnect(const SOCKET listenHandle, SOCKET *p_accept_handle);
	int Socket_Connect(const char *ip, const int port, const SOCKET client_handle);
	int Socket_Send(const SOCKET handle, const void *pv_data, const int size);
	int Socket_Recv(const SOCKET handle, void *pv_data, const int size);
	void Socket_Uninit(const SOCKET handle);

#ifdef __cplusplus
};
#endif


#endif // WINDOWS_SOCKET_H

