#ifndef SSY_LINUX_SOCKET_H
#define SSY_LINUX_SOCKET_H


#ifndef __cplusplus
#define bool    int
#define false   0
#define true    1
#endif //__cplusplus

#ifdef __cplusplus
extern "C" {
#endif

	bool InitSocketServer(const unsigned short port, int *p_listenfd);
	bool AcceptSocket(const int listenfd, int *p_connfd);
	bool InitSocketClient(const char *p_ip, const unsigned short port, int *p_connfd);
	int  SendSocket(int fd, void *p_data, int size);
	int  RecvSocket(int fd, void *p_data, int size);
	void CloseSocket(const int fd);

#ifdef __cplusplus
};
#endif


#endif // SSY_LINUX_SOCKET_H