#include "socketManager.hpp"
#include "Interface.hpp"
#include "../include/config/Config.hpp"
#include "../include/httpTransfer/httpTransfer.hpp"

std::string testHttp(const std::string &request/* , Config & config */) {
    // std::cout << "testHttp called" << std::endl;
    if (request.empty()) {
        std::cout << "request is empty" << std::endl;
        return "";
    } else {
		httpTransfer transfer(request);
		return transfer.exchange();
    }
}

std::string handle_ssl_communication(const std::string &request) {
	(void)request;
	std::string respond =
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"\r\n"
		"<!DOCTYPE html>\r\n"
		"<html>\r\n"
		"<head>\r\n"
		"<title>Hello, World!</title>\r\n"
		"</head>\r\n"
		"<body>\r\n"
		"<h1>Hello, World!</h1>\r\n"
		"</body>\r\n"
		"</html>\r\n";
	return respond;
}

int main()
{
	// int numWorker = 4;
	// parsing here
	// add application map to interface before forking the workers
	// applicationInterface::addApplication(80, /*&http::application*/);
	//Config config;
	protocolFunction testFunction = &testHttp;
	Interface::addProtocol(80, testFunction);
	protocolFunction testFunction2 = &handle_ssl_communication;
	Interface::addProtocol(443, testFunction2);
	// add sockets
	{
		t_socket socket;
		socket.port = 80;
		socket.protocol = TCP;
		socket.ipVersion = IPV4;
		socket.interfaceAddress = "0.0.0.0";
		socketManager::addSocket(socket);
		#if defined(__SSL__)
		socket.port = 443;
		socket.protocol = TCP;
		socket.ipVersion = IPV4;
		socket.interfaceAddress = "0.0.0.0";
		socket.ssl = true;
		socket.certFile = "server.crt";
		socket.keyFile = "server.key";
		socketManager::addSocket(socket);
		#endif
	}
	InterfaceFunction interfaceFunction = &Interface::interface;
	socketManager::start(interfaceFunction);
	return 0;
}
