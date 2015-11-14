#include <iostream>
#include <fstream>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
using namespace std;
#define HTTP_BUFFER_SIZE 1024*800
int main()
{
   int fd = 0, res = 0, len = 0;
   const unsigned int server_port = 80;
   const char *server_ip = "192.168.27.122";
   char getBuf[HTTP_BUFFER_SIZE] = "";
   struct sockaddr_in server;
   
   server.sin_family = AF_INET;
   server.sin_port = htons(server_port);
   inet_pton(AF_INET,server_ip,&server.sin_addr.s_addr);
   
   //create tcp socket
   fd = socket(AF_INET, SOCK_STREAM, 0);
   if(fd < 0)
   {
	   perror("socket");
	   exit(-1);
   }
   //设置套接字fd为非阻塞式IO
   
   cout << "create socket success fd: "<<fd<<endl;
   
   //connect to server...
   res = connect(fd, (struct sockaddr *)&server, sizeof(server));
   if(res != 0)
   {
	    perror("connect");
		close(fd);
		exit(-1);
   }
   
   cout << "connect to server success."<<endl;
   char buf[] = "GET./.HTTP/1.0..User-Agent:.Wget/1.11.4..Accept:.*/*..Host:.www.website1.com..Connection:.Keep-Alive....";
   char sendBuf[] = "GET www.positive.com/index.html HTTP/1.1..Accept-Language: zh-cn....";
   send(fd, sendBuf, strlen(sendBuf),0);
   memset(&getBuf,0,HTTP_BUFFER_SIZE);
   //ofstream fout;
   //fout.open("temp.txt");
   FILE *fp = fopen("temp.html", "w"); 
   //把socket设置为非阻塞模式，read函数的处理会不一样些
   //fcntl(fd, F_SETFL, O_NONBLOCK);
   int recvLen = HTTP_BUFFER_SIZE;
   //如何处理read的阻塞问题
   while(1)
   {
	   len = read(fd, getBuf,recvLen);
	   //len=recv(fd, getBuf,recvLen,0);//和read一样
	   if(len < 0)
	   {
		    // 当send收到信号时,可以继续写,但这里返回-1.
		   if(errno == EINTR)
				break;
		   // 当socket是非阻塞时,如返回此错误,表示写缓冲队列已满,
           // 在这里做延时后再重/试.
		   if(errno == EAGAIN)
		   {
			   sleep(1);
			   continue;
		   } 
	   }
	   if(fwrite(getBuf, sizeof(char), len, fp) < len) 
	   { 
			printf("temp.png Write Failed\n"); 
			break; 
	   } 
	   bzero(getBuf, HTTP_BUFFER_SIZE);
	   //加个变量就可以，如果这个变量的值变为0
	   recvLen = recvLen - len;
	   //printf("recvLen = %d\n",recvLen);
	   //printf("len = %d\n",len);
	   //printf("%s\n",getBuf);
	   //fout<<getBuf;
	   if(len == 0)
		   break;
	   //memset(&getBuf,0,HTTP_BUFFER_SIZE);
   }
   fclose(fp);
   //printf("len = %d\n",len);
   close(fd);
   //fout.close();
   
   return 0;
}
