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
	int keepAlive = 1;//����keepalive����
	int keepIdle = 60;//���60����û���κ����������������̽��
	int keepInterval = 6;//̽��ʱ������ʱ����Ϊ6��
	int keepCount = 3;//̽�Ⳣ�ԵĴ����������һ��̽������յ���Ӧ���Ժ��2�ξͲ��ٷ�
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

    int ireuseadd_on = 1;//֧�ֶ˿ڸ���
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
        printf("����˼�����...\n");
    }

    //�����̣߳����̸߳�����տͻ������ӣ����뵽epoll��
    if(-1 == pthread_create(&m_ListenThreadId, 0, ListenThread, this))
    {
        perror("pthread_create");
        exit(-1);
    }
    return true;
}
//�����߳�
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
            //��ʼ����ֻ������
            ev.events = EPOLLIN|EPOLLERR|EPOLLHUP;
            ev.data.fd = client_socket;
            epoll_ctl(pTerminalServer->m_iEpollFd, EPOLL_CTL_ADD, client_socket, &ev);
        }
    }
}
//�������
void PositiveServer::Run()
{
    int client_socket;
    char buffer[BUFFER_SIZE];
    printf("run Server\n");
	positiveHttp http;
    while(true)
    {
        //�������ȴ��¼�����
		struct epoll_event events[_MAX_SOCKFD_COUNT];
        int nfds = epoll_wait(m_iEpollFd, events, _MAX_SOCKFD_COUNT, -1);
        for(int i = 0; i < nfds; ++i)
        {
            client_socket = events[i].data.fd;
            //cout << "client  " << events[i].data.fd <<" events : "<<events[i].events<< endl;
            if(events[i].events & EPOLLIN)//���������¼�
            {
				http.recvHttpRequest(client_socket, m_iEpollFd);
            }
            //д�¼�
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