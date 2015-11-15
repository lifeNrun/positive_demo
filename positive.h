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
#include <string>
#include <string.h>
#define HTTP_PORT 80
#define MAXNUM 6
#define _MAX_SOCKFD_COUNT 2000
#define BUFFER_SIZE 512
#define HTTP_BUFFER_SIZE 1024*800
using namespace std;
//处理字符串的类
class stringOp
{
	public:
	//转换为大写
	static void transformToUpper(string &s)
	{
		if(s.empty())
			return;
		int len = s.length();
		for(int i = 0; i < len;++i)
		{
			if(s[i]>='a'&&s[i]<='z')
				s[i] = 'A' + s[i] - 'a';
		}
	}
};
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
		pthread_t m_ListenThreadId;//监听线程句柄
};

#endif