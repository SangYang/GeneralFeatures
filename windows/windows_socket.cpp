#include "windows_socket.h"
#include <stdio.h>  
#include <time.h>
#include "debug_assert.h"
#include "debug_log.h"

#pragma comment(lib, "ws2_32.lib")   

bool InitSocket(SOCKET *handle) {
	SOCKET sockHandle;
 	WORD version;  
	WSADATA wsaData;  
	int wsaRet; 
	//ULONG noBlock = 1;

	version = MAKEWORD(2, 2);  
	wsaRet = WSAStartup(version, &wsaData);
	if (NO_ERROR != wsaRet) {
		LOG("WSAStartup(2,2)=%d\n", wsaRet); 
		return false;
	}
	else {
		if (version != wsaData.wVersion) {   
			LOG("Winsock Version error=%d.%d, right=%d.%d\n",   
				HIBYTE(wsaData.wVersion), LOBYTE(wsaData.wVersion), HIBYTE(version), LOBYTE(version)); 
			WSACleanup();
			return false;
		} 
		else {
			sockHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);  
			if (INVALID_SOCKET == sockHandle) {
				LOG("socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)=INVALID_SOCKET, lastError=%d\n", WSAGetLastError());  
				WSACleanup();
				return false;
			}
			else {
				//ioctlsocket(handle, FIONBIO, &noBlock); //设置成非阻塞模式。
				*handle = sockHandle;
				return true;
			}
		}
	}
}

static bool SetSocketTimeout(const SOCKET handle, const time_t mseconds) {
	int setRet;

	setRet = setsockopt(handle, SOL_SOCKET, SO_KEEPALIVE, (const char*)&mseconds, sizeof(mseconds));
	if (-1 == setRet) {
		LOG("setsockopt() SO_KEEPALIVE failure! [%d] %s\n", errno, strerror(errno));
		return false;
	}
	setRet = setsockopt(handle, SOL_SOCKET, SO_SNDTIMEO, (const char*)&mseconds, sizeof(mseconds));   
	if (-1 == setRet) {
		LOG("setsockopt() SO_SNDTIMEO failure! [%d] %s\n", errno, strerror(errno));
		return false;
	}
	setRet = setsockopt(handle, SOL_SOCKET, SO_RCVTIMEO, (const char*)&mseconds, sizeof(mseconds));
	if (-1 == setRet) {
		LOG("setsockopt() SO_RCVTIMEO failure! [%d] %s\n", errno, strerror(errno));
		return false;
	}
	return true;
}


bool SetSocketServer(const char *ip, const int port, const SOCKET listenHandle) {
	struct sockaddr_in sockAddr;
	int sockRet;

	sockAddr.sin_family = AF_INET;  
	sockAddr.sin_port = htons(port);  
	sockAddr.sin_addr.s_addr = inet_addr(ip);  
	sockRet = bind(listenHandle, (SOCKADDR *)&sockAddr, sizeof(sockAddr));  
	if (SOCKET_ERROR == sockRet) {
		LOG("bind(%s,%d)=%d, lastError=%d\n", ip, port, sockRet, WSAGetLastError());  
		closesocket(listenHandle);
		WSACleanup();
		return false;
	}
	else {  
		sockRet = listen(listenHandle, SOMAXCONN);  
		if (SOCKET_ERROR == sockRet) {
			LOG("listen(%d)=SOCKET_ERROR, lastError=%d\n", SOMAXCONN, WSAGetLastError()); 
			closesocket(listenHandle);
			WSACleanup();
			return false;
		}
		else 
			return true;
	} 
}

bool WaitConnectSocket(const SOCKET listenHandle, SOCKET *acceptHandle) {
	SOCKET handle;
	bool okSet;

	handle = accept(listenHandle, NULL, NULL);  
	if (INVALID_SOCKET == handle) {
		LOG("accept()=INVALID_SOCKET, lastError=%d\n", WSAGetLastError()); 
		closesocket(listenHandle);
		WSACleanup();
		return false;
	}
	else {
		okSet = true;//SetSocketTimeout(handle, 60*60*24*30); 
		if (false == okSet) {
			LOG("SetSocketTimeout(%d)=false\n", 60*60*24*30);
			return false;			
		}
		else {
			*acceptHandle = handle;
			return true;		
		}
	}
}

bool ConnectSocket(const char *ip, const int port, const SOCKET connectHandle) {
	struct sockaddr_in sockAddr;
	int sockRet;
	bool okSet;

	sockAddr.sin_family = AF_INET;  
	sockAddr.sin_port = htons(port);  
	sockAddr.sin_addr.s_addr = inet_addr(ip);  
	sockRet = connect(connectHandle, (SOCKADDR *)&sockAddr, sizeof(sockAddr));
	if (SOCKET_ERROR == sockRet) {
		LOG("connect(%s,%d)=SOCKET_ERROR, lastError=%d\n", ip, port, WSAGetLastError());
		if (WSAEISCONN == WSAGetLastError()) {  // socket 已经建立连接
			return true;
		}
		else 
			return false;

	}
	else {
		okSet = true;//SetSocketTimeout(connectHandle, 500);
		if (false == okSet) {
			LOG("SetSocketTimeout(%d)=false\n", 500);
			return false;			
		}
		else {
			return true;
		}
	}
}

int SendSocket(const SOCKET handle, const void *pvData, const int size) {
	const int c_PerSize = 256;
	char *data;
	int per_size;
	int persend_size;
	int allsend_size;
	int remain_size;

	data = (char *)pvData;
	allsend_size = 0;
	while (allsend_size < size) {
		remain_size = size - allsend_size;
		if (c_PerSize > remain_size)
			per_size = remain_size;
		else 
			per_size = c_PerSize;
		persend_size = send(handle, data+allsend_size, per_size, 0);
		if (per_size != persend_size) {
			//LOG("send(%d)=%d\n", per_size, persend_size);
			break;
		}
		else
			allsend_size += persend_size;
	}
	return allsend_size;
}


int RecvSocket(const SOCKET handle, void *pvData, const int size) {
	const int c_PerSize = 256;
	char *data;
	int per_size;
	int perrecv_size;
	int allrecv_size;
	int remain_size;

	data = (char *)pvData;
	allrecv_size = 0;
	while (allrecv_size < size) {
		remain_size = size - allrecv_size;
		if (c_PerSize > remain_size)
			per_size = remain_size;	
		else 
			per_size = c_PerSize;
		perrecv_size = recv(handle, data + allrecv_size, per_size, 0);
		if (per_size != perrecv_size) {
			//LOG("recv(%d)=%d\n", per_size, perrecv_size);
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


