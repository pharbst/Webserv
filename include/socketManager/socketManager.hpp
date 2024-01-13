/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socketManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/30 15:05:15 by pharbst           #+#    #+#             */
/*   Updated: 2024/01/13 11:02:38 by pharbst          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NEW_SOCKETMANAGER_HPP
# define NEW_SOCKETMANAGER_HPP

# include <iostream>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <map>
# include <cstring>
# include <unistd.h>
# include <fcntl.h>
# include <limits.h>
# include <cerrno>
# include <csignal>
# include <sstream>

#if defined(__LINUX__) || defined(__linux__)
# include <sys/epoll.h>
# define SEPOLL socketManager::socketEpoll
#else
# include <sys/select.h>
# define SEPOLL socketManager::socketSelect
#endif



# define TCP		SOCK_STREAM
# define UDP		SOCK_DGRAM
# define LOCALHOST	AF_LOCAL
# define IPV4		AF_INET
# define IPV6		AF_INET6
# define IP			0

typedef struct s_data {
	uint32_t			port;
	uint32_t			protocol;
	const std::string	interfaceAddress;
	bool				server;
}	t_data;

typedef void	(*InterfaceFunction)(int sock, t_data sockData);

class socketManager {
	public:
		static void							addSocket(const std::string &interfaceAddress, uint32_t port, uint32_t ipVersion, uint32_t protocol);
		static void							start(InterfaceFunction interfaceFunction);
	private:
		static std::map<int, t_data>		_sockets;

		static bool							bindSocket(int fd, const std::string &interfaceAddress, uint32_t port, uint32_t ipVersion);
		static bool							validateCreationParams(const std::string &interfaceAddress, uint32_t port, uint32_t protocol);
		static void							sigHandler(int sig, siginfo_t *siginfo, void *context);
#if defined(__LINUX__) || defined(__linux__)
	static void							socketEpoll(InterfaceFunction interfaceFunction) {
		const int MAX_EVENTS = 10;
		std::map<int, uint32_t> eventsMap;

		int epollfd = epoll_create1(0);
		if (epollfd == -1) {
			std::cerr << "Error creating epoll file descriptor" << std::endl;
			return ;
		}

		struct epoll_event interest, ready[MAX_EVENTS];
		for (std::map<int, t_data>::iterator pair = _sockets.begin(); pair != _sockets.end(); pair++) {
			interest.events = EPOLLIN;
			interest.data.fd = pair->first;
			if (epoll_ctl(epollfd, EPOLL_CTL_ADD, pair->first, &interest) == -1) {
				std::cerr << "Error adding file descriptor to epoll" << std::endl;
				return ;
			}
		}

		while (true) {
			int numEvents = epoll_wait(epollfd, ready, MAX_EVENTS, -1);
			if (numEvents == -1) {
				std::cerr << "Error in epoll_wait" << std::endl;
				return ;
			}
			for (int i = 0; i < numEvents; ++i) {
				int fd = ready[i].data.fd;
				if (_sockets[fd].server)
					epollAccept(epollfd, fd);
				else
					interfaceFunction(fd, _sockets[fd]);
			}
		}
		close(epollfd);
	}
	static void							epollAccept(int &epollfd, int fd) {
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
#else
	static void							socketSelect() {
		fd_set interest;
		FD_ZERO(&interest);
		int maxfd = 0;
		for (std::map<int, t_data>::iterator pair = _sockets.begin(); pair != _sockets.end(); pair++) {
			FD_SET(pair->first, &interest);
			if (pair->first > maxfd)
				maxfd = pair->first;
		}
		while (true) {
			fd_set readyList = interest;
			int numEvents = select(maxfd + 1, &readyList, NULL, NULL, NULL);
			if (numEvents == -1) {
				std::cerr << "Error in select" << std::endl;
				return ;
			}
			for (std::map<int, t_data>::iterator it = _sockets.begin(); it != _sockets.end(); it++) {
				if (FD_ISSET(it->first, &readyList)) {
					if (it->second.server)
						selectAccept(interest, it->first, maxfd);
					else
						interfaceFunction(it->first, it->second);
				}
			}
		}
	}
	static void							selectAccept(fd_set &interest, int fd, int &maxfd) {
		while (true) {
			int newClient = accept(fd, NULL, NULL);
			if (newClient == -1) {
				if (errno == EAGAIN || errno == EWOULDBLOCK)
					break ;
				std::cout << "Error accepting connection" << std::endl;
				continue;
			}
			FD_SET(newClient, &interest);
			if (newClient > maxfd)
				maxfd = newClient;
			t_data data = _sockets[fd];
			data.server = false;
			_sockets.insert(std::pair<int, t_data>(newClient, data));
		}
	}
#endif

};

#endif