#ifndef WINDOWS_SOCKET_H
#define WINDOWS_SOCKET_H


#include <winsock2.h>

#ifdef __cplusplus
extern "C" {
#endif

	bool InitSocket(SOCKET *handle);
	bool SetSocketServer(const char *ip, const int port, const SOCKET listenHandle);
	bool WaitConnectSocket(const SOCKET listenHandle, SOCKET *p_accept_handle);
	bool ConnectSocket(const char *ip, const int port, const SOCKET client_handle);
	int  SendSocket(const SOCKET handle, const void *pv_data, const int size);
	int  RecvSocket(const SOCKET handle, void *pv_data, const int size);
	void UninitSocket(const SOCKET handle);

#ifdef __cplusplus
};
#endif


#endif // WINDOWS_SOCKET_H

