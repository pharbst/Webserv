#include "httpServer.hpp"
#include "Test.hpp"

//int socket(int domain, int type, int protocol);
// 'IPv4' defined by 'AF_INET', 'SOCK_STREAM' for bidirectional TCP traffic, '0'=default protocol for type

int main()
{
	Test test;
	test.setTest("Hello!");

	httpServer serv = httpServer("0.0.0.0", 8080);
	serv.startServer();
	serv.startListen();
	return 0;
}