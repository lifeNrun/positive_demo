#include "positiveHttp.h"
#include "positive.h"
bool positiveHttp::checkRequest(int client_socket,positive_http_header_t parseInfo)
{
	//���method
	FILE*fp;
	
	if(httpMethod.find(parseInfo.method) == -1)
	{
		printf("Bad request\n");
		http_response[client_socket] = RES_BAD_METHOD;
		return false;
	}
	
	
	int length = 0;
	char *path;
	if(parseInfo.url.length() == 1)
	{
		//requestUrl[client_socket]="";
		requestUrl[client_socket]="index.html";
		printf("checkRequest: path=index.html\n");
		return true;
	}
	else
	{
		length 	= parseInfo.url.length();
		path = (char*)malloc((length+1)*sizeof(char));
		parseInfo.url.copy(path,length,0);
		path[length] = '\0';
		//printf("path = %s\n",path);
		//���url
		if(path[0] == '/')
		{
			memmove(path,path+1,length);
			printf("checkRequest: path = %s\n",path);
			//strcpy(path,parseInfo.url);
			requestUrl[client_socket]= path;
		}
		else
		{
			printf("Bad Url\n");
			http_response[client_socket] = RES_BAD_URL;
			free(path);
			return false;
		}
	}
	

	
	if((fp=fopen(path,"r")) == NULL)
	{
		printf("Bad Url\n");
		http_response[client_socket] = RES_BAD_URL;
		free(path);
		return false;
	}
	else{
		fclose(fp);
	}
	free(path);
	//û�����������Ϊok
	http_response[client_socket] = RES_OK;
	return true;
}
void positiveHttp::recvHttpRequest(int client_socket, int EpollFd)
{
	 char buffer[HTTP_BUFFER_SIZE];
	 string httpRequest;
	 positive_http_header_t parseInfo;
	 memset(buffer, 0, sizeof(buffer));
     int rev_size = recv(client_socket, buffer, HTTP_BUFFER_SIZE,0);
	 printf("%s\n",buffer);
     if(rev_size <= 0)//�ͻ��˶Ͽ�����
     {
         //cout << "client "<< client_socket << " closed"<<endl;
         epoll_event event_del;
         event_del.data.fd = client_socket;
         event_del.events = 0;
         epoll_ctl(EpollFd, EPOLL_CTL_DEL, client_socket, &event_del);
     }
     else//���տͻ��˷��͹�������Ϣ
     {
		 httpRequest.append(buffer);
		 if(parseHttpRequest(httpRequest, &parseInfo) == 0)
			printRequest(parseInfo);
		 else{
			 printf("parseError\n");
		 }
			//��������Ϣ
		 checkRequest(client_socket ,parseInfo);
         epoll_event ev;
         //��Ӧ�ļ��������ļ����¼��޸�Ϊд
         ev.events = EPOLLOUT|EPOLLERR|EPOLLHUP;
         ev.data.fd = client_socket;//��¼���
         epoll_ctl(EpollFd, EPOLL_CTL_MOD, client_socket,&ev);
     }
}
void positiveHttp::printRequest(positive_http_header_t parseInfo)
{
	cout<<"+---------------------------------+"<<endl;
	cout<<"method:"<<parseInfo.method<<endl;
	cout<<"url:"<<parseInfo.url<<endl;
	cout<<"version:"<<parseInfo.version<<endl;
	cout<<parseInfo.other_info<<endl;
	cout<<"+---------------------------------+"<<endl;
}
//�����Է�������
int positiveHttp::parseHttpRequest(const string &request, positive_http_header_t* parseInfo)
{
	string marker1("\r\n");
	string marker2("\r\n\r\n");
	
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
	//�ҵ�
	next = request.find(marker2, prev);
	if(next != string::npos)
	{
		parseInfo->other_info= request.substr(prev,next-prev);
	}

	return 0;
}
void positiveHttp::sendError(int client_socket)
{
	char buffer[HTTP_BUFFER_SIZE];
	int length = 0;
	if(http_response[client_socket] == RES_BAD_METHOD)
	{
		strcpy(buffer,"<html><head><title>400 Bad Request</title></head><body bgcolor=\"white\"><center><h1>400 Bad Request</h1></center><hr><center>POSITIVE/1.0.0</center></body></html>");
		length = strlen(buffer)+1;
		send(client_socket, buffer, length ,0);
		return;
	}
	else if(http_response[client_socket] == RES_BAD_URL)
	{
		strcpy(buffer,"<html><head><title>404 Not Found</title></head><body bgcolor=\"white\"><center><h1>404 Not Found</h1></center><hr><center>POSITIVE/1.0.0</center></body></html>");
		length = strlen(buffer)+1;
		send(client_socket, buffer, length ,0);
	}
	
}
void positiveHttp::sendHttpResponse(int client_socket, int EpollFd)
{
	
	char buffer[HTTP_BUFFER_SIZE];
	int sendsize = 0;
	int length = 0;
	//���Ҫ���ص�����
	if(http_response[client_socket] != RES_OK)
	{
		sendError(client_socket);
	}
	else{
		//char closeConnection[] = "close";
		bzero(buffer, HTTP_BUFFER_SIZE); 
		length = getDataFromUrl(buffer, client_socket);
		sendsize = send(client_socket, buffer, length ,0);
	}
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
		epoll_event ev;
		ev.events = EPOLLIN|EPOLLERR|EPOLLHUP;
		ev.data.fd = client_socket;
		epoll_ctl(EpollFd, EPOLL_CTL_MOD,client_socket, &ev);
	}
	close(client_socket);
}

int positiveHttp::getDataFromUrl(char buffer[],int client_socket)
{
	unsigned long filesize = 0;
	struct stat statbuff;
	int length = requestUrl[client_socket].length();
	char path[length+1];
	requestUrl[client_socket].copy(path,length,0);
	path[length] = '\0';
	printf("getDataFromUrl:path = %s\n",path);
	if(stat(path,&statbuff)<0){
		return -1;
	}
	else{
		filesize = statbuff.st_size;
	}
	FILE* fp;
	fp = fopen(path,"r");
	if(fp == NULL)
	{
		perror("fopen");
		return -1;
	}
	fread(buffer,sizeof(char),filesize,fp);
	fclose(fp);
	return filesize;
}