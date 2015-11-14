#include <string>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include "positive.h"
using namespace std;
//�����http request����������ֵ
typedef struct positive_http_header_t
{
	string 		method;
	string 		url;
	string		version;
	string      other_info;
}positive_http_header_t;
enum httpResponse
{
	RES_OK = 0,
	RES_BAD_METHOD,
	RES_BAD_URL
};

class positiveHttp
{
	
	public:
	    positiveHttp(){
			httpMethod = ("GET HEAD POST");
			bzero(http_response,_MAX_SOCKFD_COUNT);
		};
		void sendError(int client_socket);
		bool checkRequest(int client_socket,positive_http_header_t parseInfo);
		int getDataFromUrl(char buffer[],int client_socket);
		void printRequest(positive_http_header_t parseInfo);
		int parseHttpRequest(const string &request, positive_http_header_t* parseInfo);
		void recvHttpRequest(int client_socket, int EpollFd);
		void sendHttpResponse(int client_socket, int EpollFd);
		~positiveHttp(){};
	private:
		string configPath;//�����ļ�·��
		string docPath;//�������ļ�·��
		string domainName;//����
		string httpMethod;
		int http_response[_MAX_SOCKFD_COUNT];
		string requestUrl[_MAX_SOCKFD_COUNT];
};
