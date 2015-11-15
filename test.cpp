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
#include <netdb.h>
#include <map>
using namespace std;

#define HTTP_BUFFER_SIZE 1024*800
void testMap()
{
	string s = "jpp";
	map<string,string> supportFiles;
	supportFiles["jpg"] = "image/jpg";
	supportFiles["jpeg"] = "image/jpeg";
	supportFiles["gif"] = "image/gif";
	supportFiles["txt"] = "text/plain";
	supportFiles["html"] = "text/html";
	if(supportFiles.find("jpp") == supportFiles.end())
		cout<<"not found"<<endl;
	exit(0);
}
void testfOpen()
{
	
	exit(0);
}
void transformToUpper(string &s)
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
void testStringFind()
{
	
	string s("GET HEAD POST");
	s[1] = 'P';
	cout<<"s = "<<s<<endl;
	string s1("get1");
	char p[10];
	char name[] = "\r\n\r\n\r\n";
	printf("length of name = %d\n",strlen(name));
    //s1.copy(p,s1.length(),0);
	transformToUpper(s1);
	cout<<s1<<endl;
	s1.clear();
	s1 = "GET";
	cout<<"clear s1 len = "<<s1.length()<<endl;
	//s.copy(p,10,0);
	int pos = s.rfind("POST",3);
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
struct temp{
	char name[10];
	int id;
};
void testCharOp()
{
   
   char *name = (char*)malloc(10);
   char str[] = "test";
   int  testNum = 100;
   strcpy(name,"yuan");
   cout<<"name is "<<name<<endl; 
   //˵����name�������ַ���Ҳ����ȡ��ַ���㳤��
   cout<<"strlen is "<<strlen(name)<<endl; 
   void *p = name;
   cout<<"1 *p = "<<*(char*)p<<endl;//*(char*)p�൱��name[0],(char*)p��ʱ�൱��name
   p = &str[1];//ָ���ַ�����ָ��
   cout<<"2 *p = "<<(char*)p<<endl;//*(char*)p�൱��str[0],(char*)p��ʱ�൱��str
   p = &testNum;
   cout<<"3 *p = "<< *(int*)p<<endl;// *(int*)p==100,p��ָ��testNum��ָ��
   memset(name,0,10);// ����дΪmemset(&name,0,10)
   cout<<"after memset strlen = "<<strlen(name)<<endl; 
   free(name);
   name = NULL;
   int num[5] = {1,2,3,4,5};
   void *s1 = num;//ָ����ʼ��ַ
   cout<<"*s1 = "<<*((int*)s1+1)<<endl;
   void *s = &num[1];//sָ��num[1]
   cout<<"void *s = "<<*(int*)s<<endl;//*(int*)sת��Ϊ��Ӧ��ָ�����ͣ���Ѱַȡֵ
   memset(&num[1],0,4*sizeof(int));// void *s = &num[1];�������ַ��ʼȫ����Ϊ0
   memset(num,0,5*sizeof(int));//==memset(&num,0,5*sizeof(int)),���������ַ
   for(int i = 0; i < 5; ++i)
	   cout<<num[i]<<"";
   cout<<endl;
   temp t;
   strcpy(t.name,"jy");
   cout<<t.name<<endl;
   cout<<t.id<<endl;
   t.id = 230972;
   memset(&t,0,sizeof(t));//����дΪmemset(t,0,sizeof(temp));��һ��������Ҫ���ݵ�ַ
   cout<<t.name<<endl;
   cout<<t.id<<endl;
   exit(0);
}
#define ACME "216.27.178.28"
#define BAIDU "180.97.33.108"
#define PICTURE "220.189.220.80"
#define LOCAL "192.168.27.122"
//����Ҫ����һ��timeout,�ɲο�wget�����run_with_timeout
void testGetHostByName()
{
	
	 hostent *host;
	 const char *ip;
	 string s;
	 char tempStr[32];
	 host = gethostbyname("cnlyml.cnblogs.com");
	 if(host == NULL)
		 exit(-1);
	 //inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str)));
	 if(host->h_addrtype == AF_INET)
	 {
		 cout<<host->h_length<<endl;
		 int i = 0;
		 while(host->h_addr_list[i] != NULL)
		 {
			 ip = inet_ntop(AF_INET, host->h_addr_list[i], tempStr, sizeof(tempStr));
			 cout<<ip<<endl;
			 i++;
		 }
	 }
	 exit(0);
	 
}
int main()
{
	//testMap();
   //testfOpen();
   //testStringFind();
   int fd = 0, res = 0, len = 0;
   const unsigned int server_port = 80;
   const char *server_ip = LOCAL;
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
   char sendBuf[] = "GET /doc/1.jpg HTTP/1.0\r\nUser-Agent: test\r\nAccept:*/*\r\nHost: 192.168.27.122\r\nConnection: Keep-Alive\r\n\r\n";
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
   char path[]="one.jpg";
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
