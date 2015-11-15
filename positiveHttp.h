#include <string>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <map>
#include "positive.h"
#define MAX_FILE_SIZE 1024*8000
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
			//bzero(requestUrl,_MAX_SOCKFD_COUNT);
			supportFiles["jpg"] = "Content-Type: image/jpg\r\n\r\n";
			supportFiles["jpeg"] = "Content-Type: image/jpeg\r\n\r\n";
			supportFiles["gif"] = "Content-Type: image/gif\r\n\r\n";
			supportFiles["txt"] = "Content-Type: text/plain\r\n\r\n";
			supportFiles["html"] = "Content-Type: text/html; charset=utf-8\r\n\r\n";
			//supportFiles = "jpg,html,txt,png";
		};
		int sendHttpHead(int client_socket,int length);
		void sendError(int client_socket);
		bool checkRequest(int client_socket,positive_http_header_t parseInfo);
		int getDataFromUrl(char buffer[],int client_socket);
		void printRequest(positive_http_header_t parseInfo);
		int parseHttpRequest(const string &request, positive_http_header_t* parseInfo);
		void recvHttpRequest(int client_socket, int EpollFd);
		void sendHttpResponse(int client_socket, int EpollFd);
		~positiveHttp(){};
	private:
		static const char badRequest[];
		static const char notFound[];
		string configPath;//�����ļ�·��
		string docPath;//�������ļ�·��
		string domainName;//����
		map<string,string> supportFiles;
		string httpMethod;
		int http_response[_MAX_SOCKFD_COUNT];
		string requestUrl[_MAX_SOCKFD_COUNT];
};
