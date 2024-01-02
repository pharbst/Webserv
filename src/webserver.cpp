#include "socketManager.hpp"

int main()
{
	socketManager::createSocket(80, IPV4 + IPV6, TCP);
	socketManager::listenSocket(80);
	return 0;
}
