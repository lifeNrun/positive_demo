#ifndef POSITIVE_H
#define POSITIVE_H
#include <sys/epoll.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#define HTTP_PORT 80
#define MAXNUM 6
#define _MAX_SOCKFD_COUNT 2000
#define BUFFER_SIZE 512
#define HTTP_BUFFER_SIZE 1024*800
class PositiveServer
{
	public:
		PositiveServer();
		~PositiveServer();
		
		bool InitServer(int iPort);
		static void *ListenThread(void * lpVoid);
		void Run();
	private:
		int   m_iEpollFd;
		int	  m_iSock;
		pthread_t m_ListenThreadId;//¼àÌýÏß³Ì¾ä±ú
};

#endif