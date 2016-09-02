#include "windows_socket.h"
#include <stdio.h>  
#include <time.h>
#include "common_log.h"

#pragma comment(lib, "ws2_32.lib")   

int Socket_Init(SOCKET *handle) {
	SOCKET sockHandle;
 	WORD version;  
	WSADATA wsaData;  
	int wsaRet; 
	//ULONG noBlock = 1;

	version = MAKEWORD(2, 2);  
	wsaRet = WSAStartup(version, &wsaData);
	if (NO_ERROR != wsaRet) {
		LOGERROR("WSAStartup(2,2)=%d\n", wsaRet); 
		return SOCK_ERR_WSASTARTUP;
	}
	else {
		if (version != wsaData.wVersion) {   
			LOGERROR("Winsock Version error=%d.%d, right=%d.%d\n",   
				HIBYTE(wsaData.wVersion), LOBYTE(wsaData.wVersion), HIBYTE(version), LOBYTE(version)); 
			WSACleanup();
			return SOCK_ERR_VERSION;
		} 
		else {
			sockHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);  
			if (INVALID_SOCKET == sockHandle) {
				LOGERROR("socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)=INVALID_SOCKET, lastError=%d\n", WSAGetLastError());  
				WSACleanup();
				return SOCK_ERR_SOCKETTCP;
			}
			else {
				//ioctlsocket(handle, FIONBIO, &noBlock); //设置成非阻塞模式。
				*handle = sockHandle;
				return SOCKE_SUCCESS;
			}
		}
	}
}

static int SetSocketTimeout(const SOCKET handle, const time_t mseconds) {
	int setRet;

	setRet = setsockopt(handle, SOL_SOCKET, SO_KEEPALIVE, (const char*)&mseconds, sizeof(mseconds));
	if (-1 == setRet) {
		LOGERROR("setsockopt() SO_KEEPALIVE failure! [%d] %s\n", errno, strerror(errno));
		return SOCK_ERR_SETKEEPALIVE;
	}
	setRet = setsockopt(handle, SOL_SOCKET, SO_SNDTIMEO, (const char*)&mseconds, sizeof(mseconds));   
	if (-1 == setRet) {
		LOGERROR("setsockopt() SO_SNDTIMEO failure! [%d] %s\n", errno, strerror(errno));
		return SOCK_ERR_SETSNDTIME;
	}
	setRet = setsockopt(handle, SOL_SOCKET, SO_RCVTIMEO, (const char*)&mseconds, sizeof(mseconds));
	if (-1 == setRet) {
		LOGERROR("setsockopt() SO_RCVTIMEO failure! [%d] %s\n", errno, strerror(errno));
		return SOCK_ERR_SETRCVTIME;
	}
	return SOCKE_SUCCESS;
}


int Socket_SetServer(const char *ip, const int port, const SOCKET listenHandle) {
	struct sockaddr_in sockAddr;
	int sockRet;

	sockAddr.sin_family = AF_INET;  
	sockAddr.sin_port = htons(port);  
	sockAddr.sin_addr.s_addr = inet_addr(ip);  
	sockRet = bind(listenHandle, (SOCKADDR *)&sockAddr, sizeof(sockAddr));  
	if (SOCKET_ERROR == sockRet) {
		LOGERROR("bind(%s,%d)=%d, lastError=%d\n", ip, port, sockRet, WSAGetLastError());  
		closesocket(listenHandle);
		WSACleanup();
		return SOCK_ERR_BIND;
	}
	else {  
		sockRet = listen(listenHandle, SOMAXCONN);  
		if (SOCKET_ERROR == sockRet) {
			LOGERROR("listen(%d)=SOCKET_ERROR, lastError=%d\n", SOMAXCONN, WSAGetLastError()); 
			closesocket(listenHandle);
			WSACleanup();
			return SOCK_ERR_LISTEN;
		}
		else 
			return SOCKE_SUCCESS;
	} 
}

int Socket_WaitConnect(const SOCKET listenHandle, SOCKET *acceptHandle) {
	SOCKET handle;
	int setRet;

	handle = accept(listenHandle, NULL, NULL);  
	if (INVALID_SOCKET == handle) {
		LOGERROR("accept()=INVALID_SOCKET, lastError=%d\n", WSAGetLastError()); 
		closesocket(listenHandle);
		WSACleanup();
		return SOCK_ERR_ACCEPT;
	}
	else {
		setRet = SOCKE_SUCCESS;//SetSocketTimeout(handle, 60*60*24*30); 
		if (SOCKE_SUCCESS != setRet) {
			LOGERROR("SetSocketTimeout(%d)=%d\n", 60*60*24*30, setRet);
			return setRet;			
		}
		else {
			*acceptHandle = handle;
			return SOCKE_SUCCESS;		
		}
	}
}

int Socket_Connect(const char *ip, const int port, const SOCKET connectHandle) {
	struct sockaddr_in sockAddr;
	int sockRet;
	int setRet;

	sockAddr.sin_family = AF_INET;  
	sockAddr.sin_port = htons(port);  
	sockAddr.sin_addr.s_addr = inet_addr(ip);  
	sockRet = connect(connectHandle, (SOCKADDR *)&sockAddr, sizeof(sockAddr));
	if (SOCKET_ERROR == sockRet) {
		LOGERROR("connect(%s,%d)=SOCKET_ERROR, lastError=%d\n", ip, port, WSAGetLastError());
		if (WSAEISCONN == WSAGetLastError()) {  // socket 已经建立连接
			return SOCK_ERR_CONNECTED;
		}
		else 
			return SOCK_ERR_CONNECT;

	}
	else {
		setRet = SOCKE_SUCCESS;//SetSocketTimeout(connectHandle, 500);
		if (SOCKE_SUCCESS != setRet) {
			LOGERROR("SetSocketTimeout(%d)=%d\n", 500, setRet);
			return setRet;			
		}
		else {
			return SOCKE_SUCCESS;
		}
	}
}

int Socket_Send(const SOCKET handle, const void *pvData, const int size) {
	const int c_PerSize = 256;
	char *data;
	int perSize;
	int sendRet;
	int retSum = 0;
	int remainSize;

	data = (char *)pvData;
	while (retSum < size) {
		remainSize = size - retSum;
		if (c_PerSize > remainSize)
			perSize = remainSize;
		else 
			perSize = c_PerSize;
		sendRet = send(handle, data+retSum, perSize, 0);
		if (0 < sendRet)
			retSum += sendRet;
		else if (0 == sendRet || retSum == size)
			break;
		else {
			LOGERROR("send(#%d,%d)=%d, lastError=%d\n", handle, perSize, sendRet, WSAGetLastError());
			return SOCK_ERR_SEND;
		}
	}
	return retSum;
}

int Socket_Recv(const SOCKET handle, void *pvData, const int size) {
	const int c_PerSize = 256;
	char *data;
	int perSize;
	int recvRet;
	int retSum = 0;
	int remainSize;

	data = (char *)pvData;
	while (retSum < size) {
		remainSize = size - retSum;
		if (c_PerSize > remainSize) 
			perSize = remainSize;	
		else 
			perSize = c_PerSize;
		recvRet = recv(handle, data+retSum, perSize, 0);
		if (0 < recvRet)
			retSum += recvRet;
		else if (0 == recvRet || retSum == size)
			break;
		else {
			LOGERROR("recv(#%d,%d)=%d, lastError=%d\n", handle, perSize, recvRet, WSAGetLastError());
			return SOCK_ERR_RECV;
		}
	}
	return retSum;
}


void Socket_Uninit(const SOCKET handle) {
	closesocket(handle); 
	WSACleanup();  
}


