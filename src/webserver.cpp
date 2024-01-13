#include "socketManager.hpp"
#include "Interface.hpp"

std::string	test(const std::string &request) {
	(void)request;
	std::cout << "test" << std::endl;
	return "test";
}

int main()
{
	// int numWorker = 4;
	// parsing here
	// add application map to interface before forking the workers
	// applicationInterface::addApplication(80, /*&http::application*/);
	protocolFunction testFunction = &test;
	Interface::addProtocol("test", testFunction);

	// add sockets
	{
		socketManager::addSocket("0.0.0.0", 80, IPV4, TCP);
	}
	InterfaceFunction interfaceFunction = &Interface::interface;
	socketManager::start(interfaceFunction);
	return 0;
}
