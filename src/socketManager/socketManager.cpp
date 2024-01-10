/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socketManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/30 15:05:28 by pharbst           #+#    #+#             */
/*   Updated: 2024/01/11 00:29:25 by pharbst          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socketManager.hpp"

std::map<int, t_data>		socketManager::_sockets;
std::map<int, t_worker>		socketManager::_workers;

void	socketManager::start() {
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_sigaction = &socketManager::sigHandler;
	sigaction(SIGUSR1, &sa, NULL);
	SEPOLL();
}

void	socketManager::sendToWorker(int fd, t_data data) {
	std::string msg = std::to_string(fd) + " " + std::to_string(data.port) + "\n";
	while (true) {
		for (std::map<int, t_worker>::iterator it = _workers.begin(); it != _workers.end(); it++) {
			if (it->second.queue == 0) {
				write(it->second.pipe, msg.c_str(), msg.length());
				it->second.queue++;
				return ;
			}
		}
		usleep(100);
	}
}

void	socketManager::addWorker(int pid, int pipe) {
	t_worker worker = {pipe, 0};
	_workers.insert(std::pair<int, t_worker>(pid, worker));
}

void	socketManager::addSocket(const std::string &interfaceAddress, uint32_t port, uint32_t ipVersion, uint32_t protocol) {
	(void)protocol;
	if (validateCreationParams(interfaceAddress, port, protocol))
		return ;
	// create socket with error check
	int fd = socket(ipVersion, protocol, 0);
	if (fd == -1) {
		std::cout << "Socket creation failed" << std::endl;
		return ;
	}
	// set socketopt to reuse address and nonblocking with error check
	{
		int flags = fcntl(fd, F_GETFL, 0);
		if (flags == -1) {
			std::cout << "Socketopt getting failed" << std::endl;
			return ;
		}
		if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
			std::cout << "Socketopt setting failed" << std::endl;
			return ;
		}
		int opt = 1;
		if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
			std::cout << "Socketopt setting failed" << std::endl;
			return ;
		}
	}
	// bind socket with error check
	if (bindSocket(fd, interfaceAddress, port, ipVersion))
		return ;
	// listen on socket with error check
	{
		if (listen(fd, 10) == -1) {
			std::cout << "Socket listening failed" << std::endl;
			return ;
		}
	}
	// set flags
	// add socket to map
	t_data data = {port, protocol, interfaceAddress, true};
	_sockets.insert(std::pair<int, t_data>(fd, data));
}
