#include "positiveHttp.h"
using namespace std;

PositiveServer::PositiveServer()
{
}

PositiveServer::~PositiveServer()
{
    close(m_iSock);
}

bool PositiveServer::InitServer(int iPort)
{
    m_iEpollFd = epoll_create1(0);
	int keepAlive = 1;//开启keepalive属性
	int keepIdle = 60;//如果60秒内没有任何数据往来，则进行探测
	int keepInterval = 6;//探测时发包的时间间隔为6秒
	int keepCount = 3;//探测尝试的次数，如果第一次探测包就收到相应，以后的2次就不再发
    /* Non-blocking I/O.  */
    if(fcntl(m_iEpollFd, F_SETFL, O_NONBLOCK) == -1)
    {
        perror("fcntl");
        exit(-1);
    }

    m_iSock = socket(AF_INET, SOCK_STREAM, 0);
    if(0 > m_iSock)
    {
        perror("socket");
        exit(1);
    }


    sockaddr_in listen_addr;
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_port = htons(iPort);
    listen_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int ireuseadd_on = 1;//支持端口复用
    //
    setsockopt(m_iSock, SOL_SOCKET, SO_REUSEADDR, &ireuseadd_on, sizeof(ireuseadd_on));
	//keepalive
	setsockopt(m_iSock,SOL_SOCKET, SO_KEEPALIVE, (void*)&keepAlive, sizeof(int));
	setsockopt(m_iSock,SOL_TCP,TCP_KEEPIDLE,(void*)&keepIdle, sizeof(int));
	setsockopt(m_iSock,SOL_TCP, TCP_KEEPINTVL,(void*)&keepInterval,sizeof(int));
	setsockopt(m_iSock,SOL_TCP, TCP_KEEPCNT,(void*)&keepCount,sizeof(int));
	
    if(-1 == bind(m_iSock, (sockaddr*)&listen_addr,sizeof(listen_addr)))
    {
        perror("bind");
        exit(-1);
    }

    if(-1 == listen(m_iSock, MAXNUM))
    {
        perror("listen");
        exit(-1);
    }
    else
    {
        printf("服务端监听中...\n");
    }

    //监听线程，此线程负责接收客户端连接，加入到epoll中
    if(-1 == pthread_create(&m_ListenThreadId, 0, ListenThread, this))
    {
        perror("pthread_create");
        exit(-1);
    }
    return true;
}
//监听线程
void *PositiveServer::ListenThread(void* lpVoid)
{
    PositiveServer *pTerminalServer = (PositiveServer*)lpVoid;
    sockaddr_in remote_addr;
    int len = sizeof(remote_addr);

    while(true)
    {
        int client_socket = accept(pTerminalServer->m_iSock,(sockaddr*)&remote_addr,(socklen_t*)&len);
        if(client_socket < 0)
        {
            perror("accept");
            continue;
        }
        else
        {
            struct epoll_event ev;
            //起始设置只监听读
            ev.events = EPOLLIN|EPOLLERR|EPOLLHUP;
            ev.data.fd = client_socket;
            epoll_ctl(pTerminalServer->m_iEpollFd, EPOLL_CTL_ADD, client_socket, &ev);
        }
    }
}
//处理程序
void PositiveServer::Run()
{
    int client_socket;
    char buffer[BUFFER_SIZE];
    printf("run Server\n");
	positiveHttp http;
    while(true)
    {
        //阻塞并等待事件出现
		struct epoll_event events[_MAX_SOCKFD_COUNT];
        int nfds = epoll_wait(m_iEpollFd, events, _MAX_SOCKFD_COUNT, -1);
        for(int i = 0; i < nfds; ++i)
        {
            client_socket = events[i].data.fd;
            //cout << "client  " << events[i].data.fd <<" events : "<<events[i].events<< endl;
            if(events[i].events & EPOLLIN)//监听到读事件
            {
				http.recvHttpRequest(client_socket, m_iEpollFd);
            }
            //写事件
            else if(events[i].events &EPOLLOUT)
            {
			
				http.sendHttpResponse(client_socket, m_iEpollFd);
            }
            else
            {
                cout << "EPOLL ERROR"<<endl;
                epoll_ctl(m_iEpollFd, EPOLL_CTL_DEL, client_socket, &events[i]);
            }
        }
		//printf("nfds : %d\n",nfds);
        if(nfds == 0)
        {
            printf("===cross===\n");
            break;
        }
    }
}
int main(int argc, char** argv)
{
    PositiveServer server;
    server.InitServer(HTTP_PORT);
    server.Run();
    return 0;
}