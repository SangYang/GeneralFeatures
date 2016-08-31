#include "linux_socket.h"
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h> 
#include <netinet/tcp.h>
#include <sys/socket.h>
#include "debug_log.h"


static void SetSocketSigpipe(void) {
	static bool bfirst = true;
	if (true == bfirst) {
		bfirst = false;
		signal(SIGPIPE, SIG_IGN);
	}
}

bool InitSocketServer(const unsigned short port, int *p_listenfd) {
	struct sockaddr_in servaddr;
	int listenfd;
	int bind_ret;
	int lstn_ret;bool ok_int ;

	SetSocketSigpipe();
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == listenfd) {
		printf("socket() error! #%d %d %s\n", listenfd, errno, strerror(errno));
		return false;
	}
	else {
		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		servaddr.sin_port = htons(port);
		bind_ret = bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
		if (-1 == bind_ret) {
			printf("bind() error! #%d %d %s\n", bind_ret, errno, strerror(errno));
			close(listenfd);
			return false;
		}
		else {
			lstn_ret = listen(listenfd, 20); // 最大接受20个连接
			if (-1 == lstn_ret) {
				printf("listen() error! #%d %d %s\n", lstn_ret, errno, strerror(errno));
				close(listenfd);
				return false;
			}
			else {
				*p_listenfd = listenfd;
				return true;
			}
		}
	}
}

static bool SetSocketTimeout(const int connfd, const time_t seconds) {
	struct timeval timeout = {seconds, 0};
	int set_ret;
	
/*-------------------------test----------------------	
	int val = 1;  
	int keepAlive = 1;
    val = 60;  
    if (setsockopt(connfd, IPPROTO_TCP, TCP_KEEPIDLE, &val, sizeof(val)) < 0) {  
        printf("setsockopt TCP_KEEPIDLE: %s\n", strerror(errno));  
        return false;  
    }  
    val = 60/3;  
    if (val == 0) 
		val = 1;  
    if (setsockopt(connfd, IPPROTO_TCP, TCP_KEEPINTVL, &val, sizeof(val)) < 0) {  
        printf("setsockopt TCP_KEEPINTVL: %s\n", strerror(errno));  
        return false;  
    }  
    val = 3;  
    if (setsockopt(connfd, IPPROTO_TCP, TCP_KEEPCNT, &val, sizeof(val)) < 0) {  
        printf("setsockopt TCP_KEEPCNT: %s\n", strerror(errno));  
        return false;  
    } 
	if(setsockopt(connfd, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(keepAlive)) == -1)
	{
        printf("setsockopt SO_KEEPALIVE: %s\n", strerror(errno));
		return false;
	}	
-------------------------------test_end---------------*/	
	
	set_ret = setsockopt(connfd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));   
	if (-1 == set_ret) {
		LOG("setsockopt() SendTimeOut failure! [%d] %s\n", errno, strerror(errno));
		return false;
	}
	else {
		set_ret = setsockopt(connfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
		if (-1 == set_ret) {
			LOG("setsockopt() RecvTimeOut failure! [%d] %s\n", errno, strerror(errno));
			return false;
		}
		else {
			return true;
		}
	}
}

bool AcceptSocket(const int listenfd, int *p_connfd) {
	struct sockaddr_in cliaddr;
	socklen_t cliaddr_len;
	int connfd;
	bool ok_set;

	cliaddr_len = sizeof(cliaddr);
	connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddr_len);
	if (-1 == connfd) {
		printf("accept() error! #%d %d %s\n", connfd, errno, strerror(errno));
		return false;
	}
	else {
		LOG("accept() success! #%d %s:%d\n", connfd, 
			(char *)inet_ntoa(cliaddr.sin_addr), (unsigned short)ntohs(cliaddr.sin_port));
		ok_set = true;//SetSocketTimeout(connfd, 60); // 超时 10 秒
		if (false == ok_set) {
			printf("SetSocketTimeout() error!\n");
			close(connfd);				
			return false;			
		}
		else {
			*p_connfd = connfd;
			return true;			
		}
	}
}

void CloseSocket(const int fd) {
	int cls_ret;

	cls_ret = close(fd);
	if (-1 == cls_ret) {
		LOG("close() error! fd=%d\n", fd);
	}
	else {
		LOG("close() success! fd=%d\n", fd);		
	}
}

bool InitSocketClient(const char *p_ip, const unsigned short port, int *p_connfd) {
	struct sockaddr_in servaddr;
	int sockfd;
	int conn_ret;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == sockfd) {
		LOG("socket() error! #%d %d %s\n", sockfd, errno, strerror(errno));
		return false;
	}
	else {
		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		inet_pton(AF_INET, p_ip, &servaddr.sin_addr);
		servaddr.sin_port = htons(port);	
		conn_ret = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
		if (-1 == conn_ret) {
			LOG("connect() error! #%d %d %s\n", conn_ret, errno, strerror(errno));
			close(sockfd);
			return false;
		}
		else {
			*p_connfd = sockfd;
			return true;			
		}
	}
}

int SendSocket(int fd, void *p_data, int size) {
	int all_len;
	int per_len;

	all_len = 0;
	while (all_len < size) {
		per_len = send(fd, (char *)p_data + all_len, size - all_len, 0);
		if (-1 == per_len) {
			//LOG("send() #%d %d %s\n", fd, errno, strerror(errno));
			return -1;			
		}
		else
			all_len += per_len;
	}
	return all_len;
}

int RecvSocket(int fd, void *p_data, int size) {
	int all_len;
	int per_len;

	all_len = 0;
	while (all_len < size) {
		per_len = recv(fd, (char *)p_data + all_len, size - all_len, 0);
		if (-1 == per_len) {
			//LOG("recv() #%d %d %s\n", fd, errno, strerror(errno));
			return -1;			
		}
		else if (0 == per_len && 0 != size) {
			//LOG("recv() #%d %d %s\n", fd, errno, strerror(errno));			
			return -1;
		}
		else
			all_len += per_len;
	}
	return all_len;
}


#if 0
int main(void) {
	int listenfd;
	int connfd;
	char buffer[256] = "ABCDEF";
	int send_len;
	int recv_len;
	bool ok_init;
	bool ok_accept;

	ok_init = InitSocketServer(8552, &listenfd);
	if (false == ok_init) {
		LOG("InitSocketServer() error!\n");
	}
	else {
		LOG(">>Accepting connections...\n");
		while (true){
			ok_accept = AcceptSocket(listenfd, &connfd);
			if (false == ok_accept) {
				LOG("AcceptSocket() error!\n");
			}
			else {
				strcpy(buffer, "abcdef");				
				send_len = SendSocket(connfd, buffer, strlen(buffer) + 1);	
				LOG("send_len = %d, buffer=%s\n", send_len, buffer);
				recv_len = RecvSocket(connfd, buffer, strlen(buffer) + 1);
				LOG("recv_len = %d, buffer=%s\n", recv_len, buffer);

				close(connfd);				
			}
		}
		close(listenfd);
	}

	return 0;
}
#endif
