#include "socketManager.hpp"

int main()
{
	socketManager::addSocket("127.0.0.1", 8080, IPV4, TCP);
	return 0;
}
