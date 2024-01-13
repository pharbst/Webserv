/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socketManagerTools.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/30 15:12:07 by pharbst           #+#    #+#             */
/*   Updated: 2024/01/13 17:31:34 by pharbst          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socketManager.hpp"

bool	socketManager::validateCreationParams(const std::string &interfaceAddress, uint32_t port, uint32_t protocol) {
	// validate port
	{
		if (port > 0XFFFF) {
			std::cout << "Port is out of range" << std::endl;
			return true;
		}
	}
	// validate protocol
	{
		switch (protocol)
		{
		case TCP:
			break;
		case UDP:
			break;
		default:
			return true;
		}
	}
	// check if socket already exists
	{
		for (std::map<int, t_data>::iterator it = _sockets.begin(); it != _sockets.end(); it++) {
			if (it->second.port == port && it->second.protocol == protocol && it->second.interfaceAddress == interfaceAddress) {
				std::cout << "Socket already exists" << std::endl;
				return true;
			}
		}
	}
	return false;
}

bool	socketManager::bindSocket(int fd, const std::string &interfaceAddress, uint32_t port, uint32_t ipVersion) {
		if (ipVersion == IPV4) {
			sockaddr_in addr;
			addr.sin_family = AF_INET;
			addr.sin_port = htons(port);
			if (inet_pton(AF_INET, interfaceAddress.c_str(), &addr.sin_addr) != 1) {
				std::cout << "Invalid interface address" << std::endl;
				return true;
			}
			if (bind(fd, (sockaddr *)&addr, sizeof(addr)) == -1) {
				std::cout << "Socket binding failed" << std::endl;
				return true;
			}
		}
		else if (ipVersion == IPV6) {
			sockaddr_in6 addr;
			addr.sin6_family = AF_INET6;
			addr.sin6_port = htons(port);
			if (inet_pton(AF_INET6, interfaceAddress.c_str(), &addr.sin6_addr) != 1) {
				std::cout << "Invalid interface address" << std::endl;
				return true;
			}
			if (bind(fd, (sockaddr *)&addr, sizeof(addr)) == -1) {
				std::cout << "Socket binding failed" << std::endl;
				return true;
			}
		}
		else {
			std::cout << "Invalid ipVersion" << std::endl;
			return true;
		}
	return false;
}

#if defined(__LINUX__) || defined(__linux__)
int								socketManager::_epollfd = -1;
void							socketManager::socketEpoll(InterfaceFunction interfaceFunction) {
	const int MAX_EVENTS = 10;
	std::map<int, uint32_t> eventsMap;

	_epollfd = epoll_create1(0);
	if (_epollfd == -1) {
		std::cerr << "Error creating epoll file descriptor" << std::endl;
		return ;
	}

	struct epoll_event interest, ready[MAX_EVENTS];
	for (std::map<int, t_data>::iterator pair = _sockets.begin(); pair != _sockets.end(); pair++) {
		interest.events = EPOLLIN;
		interest.data.fd = pair->first;
		if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, pair->first, &interest) == -1) {
			std::cerr << "Error adding file descriptor to epoll" << std::endl;
			return ;
		}
	}

	while (true) {
		int numEvents = epoll_wait(_epollfd, ready, MAX_EVENTS, -1);
		if (numEvents == -1) {
			std::cerr << "Error in epoll_wait" << std::endl;
			return ;
		}
		for (int i = 0; i < numEvents; ++i) {
			int fd = ready[i].data.fd;
			if (_sockets[fd].server)
				epollAccept(_epollfd, fd);
			else
				interfaceFunction(fd, _sockets[fd]);
		}
	}
	close(_epollfd);
	_epollfd = -1;
}
void							socketManager::epollAccept(int fd) {
	while (true) {
		int newClient = accept(fd, NULL, NULL);
		if (newClient == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				break ;
			std::cout << "Error accepting connection" << std::endl;
			continue;
		}
		struct epoll_event ev;
		ev.events = EPOLLIN;
		ev.data.fd = newClient;
		if (epoll_ctl(epollfd, EPOLL_CTL_ADD, newClient, &ev) == -1) {
			std::cerr << "Error adding file descriptor to epoll" << std::endl;
			return ;
		}
		t_data data = _sockets[fd];
		data.server = false;
		_sockets.insert(std::pair<int, t_data>(newClient, data));
	}
}
void							socketManager::epollRemove(int fd) {
	if (epoll_ctl(_epollfd, EPOLL_CTL_DEL, fd, NULL) == -1) {
		std::cerr << "Error removing file descriptor from epoll" << std::endl;
		return ;
	}
}
#else
fd_set							socketManager::_interest;
int								socketManager::_maxfd = -1;
void							socketManager::socketSelect(InterfaceFunction interfaceFunction) {
	FD_ZERO(&_interest);
	int maxfd = 0;
	for (std::map<int, t_data>::iterator pair = _sockets.begin(); pair != _sockets.end(); pair++) {
		FD_SET(pair->first, &_interest);
		if (pair->first > maxfd)
			maxfd = pair->first;
	}
	while (true) {
		fd_set readyList = _interest;
		int numEvents = select(maxfd + 1, &readyList, NULL, NULL, NULL);
		if (numEvents == -1) {
			std::cerr << "Error in select" << std::endl;
			return ;
		}
		for (std::map<int, t_data>::iterator it = _sockets.begin(); it != _sockets.end(); it++) {
			if (FD_ISSET(it->first, &readyList)) {
				if (it->second.server)
					selectAccept(it->first);
				else
					interfaceFunction(it->first, it->second);
			}
		}
	}
}
void							socketManager::selectAccept(int fd) {
	while (true) {
		int newClient = accept(fd, NULL, NULL);
		if (newClient == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				break ;
			std::cout << "Error accepting connection" << std::endl;
			continue;
		}
		FD_SET(newClient, &_interest);
		if (newClient > _maxfd)
			_maxfd = newClient;
		t_data data = _sockets[fd];
		data.server = false;
		_sockets.insert(std::pair<int, t_data>(newClient, data));
	}
}
void							socketManager::selectRemove(int fd) {
	FD_CLR(fd, &_interest);
	if (fd == _maxfd) {
		for (std::map<int, t_data>::iterator it = _sockets.begin(); it != _sockets.end(); it++) {
			if (it->first > _maxfd)
				_maxfd = it->first;
		}
	}
}
#endif
