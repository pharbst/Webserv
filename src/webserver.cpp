#include "socketManager.hpp"

int main()
{
	int numWorker = 4;
	// parsing here
	// add application map to interface before forking the workers
	// applicationInterface::addApplication(80, /*&http::application*/);

	// start workers
	{
		for (int i = numWorker; i > 0; i--) {
			int workerPipe[2];
			if (pipe(workerPipe) == -1) {
				std::cout << "Error: Pipe failed" << std::endl;
				return 1;
			}
			int pid = fork();
			if (pid == -1) {
				std::cout << "Error: Fork failed" << std::endl;
				return 1;
			}
			if (pid == 0) {
				close(workerPipe[0]);
				// applicationInterface::Interface(workerPipe[1]);
				return 0;
			}
			close(workerPipe[1]);
			socketManager::addWorker(pid, workerPipe[0]);
		}
	}

	// add sockets
	{
		socketManager::addSocket("0.0.0.0", 80, IPV4, TCP);
	}
	// socketManager::start();
	return 0;
}
