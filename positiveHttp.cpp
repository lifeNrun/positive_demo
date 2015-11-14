#include "positiveHttp.h"
#include "positive.h"
void positiveHttp::recvHttpRequest(int client_socket, int EpollFd)
{
	 char buffer[HTTP_BUFFER_SIZE];
	 string httpRequest;
	 positive_http_header_t parseInfo;
	 memset(buffer, 0, sizeof(buffer));
     int rev_size = recv(client_socket, buffer, HTTP_BUFFER_SIZE,0);
	 
     if(rev_size <= 0)//客户端断开连接
     {
         //cout << "client "<< client_socket << " closed"<<endl;
         epoll_event event_del;
         event_del.data.fd = client_socket;
         event_del.events = 0;
         epoll_ctl(EpollFd, EPOLL_CTL_DEL, client_socket, &event_del);
     }
     else//接收客户端发送过来的消息
     {
		 httpRequest.append(buffer);
		 if(parseHttpRequest(httpRequest, &parseInfo) == 0)
			printRequest(parseInfo);
         epoll_event ev;
         //对应文件描述符的监听事件修改为写
         ev.events = EPOLLOUT|EPOLLERR|EPOLLHUP;
         ev.data.fd = client_socket;//记录句柄
         epoll_ctl(EpollFd, EPOLL_CTL_MOD, client_socket,&ev);
     }
}
void positiveHttp::printRequest(positive_http_header_t parseInfo)
{
	cout<<"+---------------------------------+"<<endl;
	cout<<"method:"<<parseInfo.method<<endl;
	cout<<"url:"<<parseInfo.url<<endl;
	cout<<"version:"<<parseInfo.version<<endl;
	cout<<"language:"<<parseInfo.language<<endl;
	cout<<"+---------------------------------+"<<endl;
}
int positiveHttp::parseHttpRequest(const string &request, positive_http_header_t* parseInfo)
{
	string marker1("..");
	string marker2("....");
	
	if(request.empty())
	{
		printf("http request is empty\n");
		return -1;
	}
	if(NULL == parseInfo)
	{
		printf("parseInfo is NULL\n");
		return -1;
	}
	int prev = 0, next = 0;
	next = request.find(marker1,prev);
	if(next != string::npos)
	{
		string GETPart(request.substr(prev,next-prev));
		prev = next + marker1.length();
		stringstream sstream(GETPart);
		sstream >> (parseInfo->method);
		sstream >> (parseInfo->url);
		sstream >> (parseInfo->version);
	}
	//找到....
	next = request.find(marker2, prev);
	if(next != string::npos)
	{
		parseInfo->language= request.substr(prev,next-prev);
	}
	return 0;
}

void positiveHttp::sendHttpResponse(int client_socket, int EpollFd)
{
	char buffer[HTTP_BUFFER_SIZE];
	//FILE *fp = fopen("test.png","r");
	//if(fp == NULL)
	//{
	//	perror("fopen");
	//	return;
	//}
		
	int sendsize = 0;
	int length = 0;
	//char closeConnection[] = "close";
	bzero(buffer, HTTP_BUFFER_SIZE); 
	getDataFromUrl(buffer);
	sendsize = send(client_socket, HTTP_BUFFER_SIZE, length ,0);
    //sprintf(buffer, "Hello, Client fd: %d",client_socket);
	//printf("sizeof(buffer)=%d\n",sizeof(buffer));
	/*
	while(1)
	{
		length = fread(buffer,sizeof(char),HTTP_BUFFER_SIZE,fp);
		if(length <= 0)
			break;
		sendsize = send(client_socket, buffer, length ,0);
		//printf("sendsize=%d\n",sendsize);
		if(sendsize<0)
		{
			perror("send");
			break;
		}
		bzero(buffer, HTTP_BUFFER_SIZE); 
		//send(client_socket, closeConnection,strlen(closeConnection)+1,MSG_NOSIGNAL);
	}
	*/
	//fclose(fp);
	if(sendsize <= 0)
	{
		struct epoll_event event_del;
		event_del.data.fd = client_socket;
		event_del.events = 0;
		epoll_ctl(EpollFd, EPOLL_CTL_DEL, client_socket, &event_del);
	}
	else
	{
		//printf("Server reply msg ok. msg: %s\n",buffer);
		close(client_socket);
		epoll_event ev;
		ev.events = EPOLLIN|EPOLLERR|EPOLLHUP;
		ev.data.fd = client_socket;
		epoll_ctl(EpollFd, EPOLL_CTL_MOD,client_socket, &ev);
	}
}

int positiveHttp::getDataFromUrl(char buffer[])
{
	ifstream inf;
	
	inf.open("index.html");
	//inf>>buffer;
	inf.getline(buffer, HTTP_BUFFER_SIZE,0);
	//printf("%s\n",buffer);
	inf.close();
}