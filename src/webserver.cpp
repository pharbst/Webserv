#include "socketManager.hpp"
#include "Interface.hpp"

std::string	testHttp(const std::string &request) {
	(void)request;
	std::cout << "testHttp called" << std::endl;
	return "HTTP/1.1 200 OK\r\n\r\nHello World!";
}

int main()
{
	// int numWorker = 4;
	// parsing here
	// add application map to interface before forking the workers
	// applicationInterface::addApplication(80, /*&http::application*/);
	protocolFunction testFunction = &testHttp;
	Interface::addProtocol("test", testFunction);

	// add sockets
	{
		socketManager::addSocket("0.0.0.0", 80, IPV4, TCP);
	}
	InterfaceFunction interfaceFunction = &Interface::interface;
	socketManager::start(interfaceFunction);
	return 0;
}
