#include <iostream>
#include <fstream>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <fcntl.h>
using namespace std;
#define HTTP_BUFFER_SIZE 1024*800
void testStringFind()
{
	string s("GET HEAD POST");
	char name[] = "\r\n\r\n\r\n";
	printf("length of name = %d\n",strlen(name));
	char p[10];
	s.copy(p,10,0);
	int pos = s.find("GET",0);
	printf("pos=%d\n",pos);
	exit(0);
}
int getContentLength(string s)
{
	int pos1 = 0;
	int pos2 = 0;
	int length = 0;
	char num[33];
	int titleLength = strlen("Content-Length: ");
	pos1 = s.find("Content-Length:",0);
	pos2 = s.find("\r\n");
	s.copy(num,pos2-titleLength-pos1,pos1+titleLength);
	length = atoi(num);
	
    return length;
}
#define ACME "216.27.178.28"
#define BAIDU "180.97.33.108"
#define PICTURE "220.189.220.80"
int main()
{
   //testStringFind();
   int fd = 0, res = 0, len = 0;
   const unsigned int server_port = 80;
   const char *server_ip = PICTURE;
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
   //�����׽���fdΪ������ʽIO
   
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
   //char buf[] = "GET./.HTTP/1.0..User-Agent:.Wget/1.11.4..Accept:.*/*..Host:.www.website1.com..Connection:.Keep-Alive....";
   char sendBuf[] = "GET /pic/0/11/10/17/11101745_83165866.jpg HTTP/1.0\r\nUser-Agent: test\r\nAccept:*/*\r\nHost: yynews.cnnb.com.cn\r\nConnection: Keep-Alive\r\n\r\n";
   char  sendBuf2[] = "GET /show/1/9/7491202kee3a48b1.html HTTP/1.0\r\nUser-Agent: test\r\nAccept:*/*\r\nHost: www.nipic.com\r\nConnection: Keep-Alive\r\n\r\n";
   send(fd, sendBuf, strlen(sendBuf),0);
   
   int recvLen = HTTP_BUFFER_SIZE;
   bzero(getBuf, HTTP_BUFFER_SIZE);
   len=read(fd, getBuf,recvLen);
   cout<<"read len "<<len<<endl;
   string s(getBuf);
  
   //��ȡhttpͷ��β��λ��
   int pos = s.find("\r\n\r\n");
   //��ȡhttpͷ��Ϣ
   string sout = s.substr(0,pos+4);
   cout<<sout;
   //cout<<sout<<endl;
   cout<<s.length()<<endl;
   //�������ݵĳ���
   int otherDataLen = len - pos -4;
   cout<<"otherDataLen = "<<otherDataLen<<endl;
   //��ȡ�������ݲ�д���ļ�
   char temp[otherDataLen];
   bzero(temp, otherDataLen);
   memcpy(temp,getBuf+pos+4,otherDataLen);
   char path[]="11101745_831658.jpg";
   FILE *fp = fopen(path, "w"); 
   //�����404����Ͱ��ļ�ɾ�������ر�socket���ļ�
   if(s.find("404")!=-1)
   {
	    fclose(fp);
		close(fd);
	    if(remove(path) != 0)
		{
			perror("remove");
		}
		return -1;
   }
   if(fwrite(temp, sizeof(char), otherDataLen, fp) < otherDataLen) 
   { 

		printf("Write Failed\n"); 
		
		
		return -1;
   } 

   bzero(getBuf, HTTP_BUFFER_SIZE);
   //��socket����Ϊ������ģʽ��read�����Ĵ���᲻һ��Щ
   //fcntl(fd, F_SETFL, O_NONBLOCK);
   //��δ���read����������
   //������fd��ȡ���ݣ���д���ļ�
   while(1)
   {
	   len = read(fd, getBuf,recvLen);
	   //len=recv(fd, getBuf,recvLen,0);//��readһ��
	   if(len < 0)
	   {
		    // ��send�յ��ź�ʱ,���Լ���д,�����ﷵ��-1.
		   if(errno == EINTR)
				break;
		   // ��socket�Ƿ�����ʱ,�緵�ش˴���,��ʾд�����������,
           // ����������ʱ������/��.
		   if(errno == EAGAIN)
		   {
			   sleep(1);
			   continue;
		   } 
	   }
	   if(fwrite(getBuf, sizeof(char), len, fp) < len) 
	   { 
			printf("temp.html Write Failed\n"); 
			break; 
	   } 
	   bzero(getBuf, HTTP_BUFFER_SIZE);
	   printf("len = %d\n",len);
	   if(len == 0)
		   break;
   }
   fclose(fp);
   close(fd);
   return 0;
}
