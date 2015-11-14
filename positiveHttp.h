
#include <string>
#include <sstream>
#include <fstream>
using namespace std;
//保存从http request解析下来的值
typedef struct positive_http_header_t
{
	string 		method;
	string 		url;
	string		version;
	string      language;
	string 		body;
}positive_http_header_t;
class positiveHttp
{
	
	public:
	    positiveHttp(){};
		int getDataFromUrl(char buffer[]);
		void printRequest(positive_http_header_t parseInfo);
		int parseHttpRequest(const string &request, positive_http_header_t* parseInfo);
		void recvHttpRequest(int client_socket, int EpollFd);
		void sendHttpResponse(int client_socket, int EpollFd);
		~positiveHttp(){};
	private:
		string configPath;//配置文件路径
		string docPath;//服务器文件路径
		string domainName;//域名
};
