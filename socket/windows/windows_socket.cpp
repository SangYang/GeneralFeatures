#include "stdafx.h"
#include "windows_socket.h"
#include <stdio.h>  
#include <time.h>
#include "debug_log.h"

#pragma comment(lib, "ws2_32.lib")  

static t_Socket  g_socket; 

void g_GetSocket(t_Socket *p_socket) {
	ASSERT(NULL != p_socket);
	p_socket->m_handle = g_socket.m_handle;
	p_socket->m_port = g_socket.m_port;
	strcpy(p_socket->m_ip, g_socket.m_ip);
}

void g_SetSocket(const t_Socket socket) {	
	g_socket.m_handle = socket.m_handle;
	g_socket.m_port = socket.m_port;
	strcpy(g_socket.m_ip, socket.m_ip);
}

bool InitSocket(SOCKET *p_handle) {
	SOCKET handle;
 	WORD version;  
	WSADATA wsa_data;  
	int wsa_ret; 
	//ULONG noBlock = 1;

	ASSERT(NULL != p_handle);
	version = MAKEWORD(2, 2);  
	wsa_ret = WSAStartup(version, &wsa_data);
	if (NO_ERROR != wsa_ret) {
		LOG("WSAStartup() error = %d", wsa_ret); 
		return false;
	}
	else if (version != wsa_data.wVersion) {   
		LOG("Winsock Version error = %d.%d, right = %d.%d",   
			HIBYTE(wsa_data.wVersion), LOBYTE(wsa_data.wVersion), HIBYTE(version), LOBYTE(version)); 
		return false;
	} 
	else {
		handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);  
		if (INVALID_SOCKET == handle) {
			LOG("socket() error = %d", WSAGetLastError());  
			return false;
		}
		else {
			//ioctlsocket(handle, FIONBIO, &noBlock); //设置成非阻塞模式。
			*p_handle = handle;
			return true;
		}
	}
}

static bool SetSocketTimeout(const SOCKET handle, const time_t mseconds) {
	int set_ret;

	set_ret = setsockopt(handle, SOL_SOCKET, SO_KEEPALIVE, (const char*)&mseconds, sizeof(mseconds));
	if (-1 == set_ret) {
		printf("setsockopt() SO_KEEPALIVE failure! [%d] %s\n", errno, strerror(errno));
		return false;
	}
	set_ret = setsockopt(handle, SOL_SOCKET, SO_SNDTIMEO, (const char*)&mseconds, sizeof(mseconds));   
	if (-1 == set_ret) {
		printf("setsockopt() SO_SNDTIMEO failure! [%d] %s\n", errno, strerror(errno));
		return false;
	}
	set_ret = setsockopt(handle, SOL_SOCKET, SO_RCVTIMEO, (const char*)&mseconds, sizeof(mseconds));
	if (-1 == set_ret) {
		printf("setsockopt() SO_RCVTIMEO failure! [%d] %s\n", errno, strerror(errno));
		return false;
	}
	return true;
}


bool SetSocketServer(const char *p_ip, const int port, const SOCKET listen_handle) {
	struct sockaddr_in socket_addr;
	int sock_ret;

	ASSERT(NULL != p_ip);
	socket_addr.sin_family = AF_INET;  
	socket_addr.sin_port = htons(port);  
	socket_addr.sin_addr.s_addr = inet_addr(p_ip);  
	sock_ret = bind(listen_handle, (SOCKADDR *)&socket_addr, sizeof(socket_addr));  
	if (SOCKET_ERROR == sock_ret) {
		LOG("bind() error = %d", WSAGetLastError());  
		return false;
	}
	else {  
		sock_ret = listen(listen_handle, SOMAXCONN);  
		if (SOCKET_ERROR == sock_ret) {
			LOG("listen() error = %d", WSAGetLastError());  
			return false;
		}
		else 
			return true;
	} 
}

bool WaitConnectSocket(const SOCKET listen_handle, SOCKET *p_accept_handle) {
	SOCKET accept_handle;
	bool ok_set;

	ASSERT(NULL != p_accept_handle);
	accept_handle = accept(listen_handle, NULL, NULL);  
	if (INVALID_SOCKET == accept_handle) {
		LOG("accept() error = %d", WSAGetLastError()); 
		return false;
	}
	else {
		ok_set = true;//SetSocketTimeout(accept_handle, 60 * 60 * 24 * 30); 
		if (false == ok_set) {
			printf("SetSocketTimeout() error!\n");
			closesocket(accept_handle); 
			return false;			
		}
		else {
			*p_accept_handle = accept_handle;
			return true;		
		}
	}
}

bool ConnectSocket(const char *p_ip, const int port, const SOCKET client_handle) {
	struct sockaddr_in socket_addr;
	int sock_ret;
	bool ok_set;

	ASSERT(NULL != p_ip);	
	ok_set = true;//SetSocketTimeout(client_handle, 500);
	if (false == ok_set) {
		printf("SetSocketTimeout() error!\n");
		return false;			
	}
	else {
		socket_addr.sin_family = AF_INET;  
		socket_addr.sin_port = htons(port);  
		socket_addr.sin_addr.s_addr = inet_addr(p_ip);  
		sock_ret = connect(client_handle, (SOCKADDR *)&socket_addr, sizeof(socket_addr));  
		if (SOCKET_ERROR == sock_ret) {
			LOG("connect() error = %d", WSAGetLastError());
/*
			if (WSAEISCONN == WSAGetLastError()) {  // socket 已经建立连接
				return true;
			}
			else*/
				return false;
		}
		else {
			return true;
		}
	}
}

int SendSocket(const SOCKET handle, const void *pv_data, const int size) {
	const int c_PerSize = 256;
	char *p_data;
	int per_size;
	int persend_size;
	int allsend_size;
	int remain_size;

	ASSERT(NULL != pv_data && 0 < size);
	p_data = (char *)pv_data;
	allsend_size = 0;
	while (allsend_size < size) {
		remain_size = size - allsend_size;
		if (c_PerSize > remain_size)
			per_size = remain_size;
		else 
			per_size = c_PerSize;
		persend_size = send(handle, p_data + allsend_size, per_size, 0);
		if (per_size != persend_size) {
			LOG("send() error = %d", persend_size);
			break;
		}
		else
			allsend_size += persend_size;
	}
	return allsend_size;
}


int RecvSocket(const SOCKET handle, void *pv_data, const int size) {
	const int c_PerSize = 256;
	char *p_data;
	int per_size;
	int perrecv_size;
	int allrecv_size;
	int remain_size;

	ASSERT(NULL != pv_data && 0 < size);
	p_data = (char *)pv_data;
	allrecv_size = 0;
	while (allrecv_size < size) {
		remain_size = size - allrecv_size;
		if (c_PerSize > remain_size)
			per_size = remain_size;	
		else 
			per_size = c_PerSize;
		perrecv_size = recv(handle, p_data + allrecv_size, per_size, 0);
		if (per_size != perrecv_size) {
			//LOG("recv() error = %d", perrecv_size);
			break;
		}
		else
			allrecv_size += perrecv_size;
	}
	return allrecv_size;
}


void UninitSocket(const SOCKET handle) {
	closesocket(handle); 
	WSACleanup();  
}


