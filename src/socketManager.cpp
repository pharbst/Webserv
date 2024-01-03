/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socketManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/30 15:05:28 by pharbst           #+#    #+#             */
/*   Updated: 2024/01/03 19:23:48 by pharbst          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socketManager.hpp"

std::map<uint32_t, t_socket>	socketManager::_sockets;
// std::map<int, uint32_t>		socketManager::_clientSockets;

void	socketManager::createSocket(uint32_t port, uint32_t ipVersion, uint32_t protocol) {
	// init
	(void)protocol;
	uint16_t	interface = socketManager::validateCreationParams(port, ipVersion);
	if (interface == 1)
		return ;
	// create sockets
	t_socket sock = socketManager::createSockets(interface);
	// check socket creation
	uint32_t socketNum = socketManager::checkSocketCreation(sock, interface, port);
	// set reuse address when more than one socket was created
	if (socketNum > 1)
		socketManager::setReuseAddr(sock);
	// bind sockets
	socketManager::bindSockets(sock, port);
	// listen sockets
	socketManager::listenSocket(sock, port);
	// add node to _sockets
	_sockets.insert(std::pair<uint32_t, t_socket>(port, sock));
}

void	socketManager::listenSocket(t_socket &sock, const uint32_t port) {
	if (sock.ipv4 > 0) {
		if (listen(sock.ipv4, 10) < 0) {
			std::cout << "listen() failed for ipv4 socket" << std::endl;
			sock.ipv4 = 0;
			memset((void *)&sock.ipv4Addr, 0, sizeof(sock.ipv4Addr));
			return ;
		}
		int flags = fcntl(sock.ipv4, F_GETFL, 0);
		if (flags < 0) {
			std::cout << "fcntl() failed for ipv4 socket" << std::endl;
			sock.ipv4 = 0;
			memset((void *)&sock.ipv4Addr, 0, sizeof(sock.ipv4Addr));
			return ;
		}
		if (fcntl(sock.ipv4, F_SETFL, flags | O_NONBLOCK) < 0) {
			std::cout << "ipv4 socket cant be set to nonblocking" << std::endl;
			sock.ipv4 = 0;
			memset((void *)&sock.ipv4Addr, 0, sizeof(sock.ipv4Addr));
			return ;
		}
		std::cout << "ipv4 socket is listening on port: " << port << std::endl;
	}
	if (sock.ipv6 > 0) {
		if (listen(sock.ipv6, 10) < 0) {
			std::cout << "listen() failed for ipv6 socket" << std::endl;
			sock.ipv6 = 0;
			memset((void *)&sock.ipv6Addr, 0, sizeof(sock.ipv6Addr));
			return ;
		}
		int flags = fcntl(sock.ipv6, F_GETFL, 0);
		if (flags < 0) {
			std::cout << "fcntl() failed for ipv6 socket" << std::endl;
			sock.ipv6 = 0;
			memset((void *)&sock.ipv6Addr, 0, sizeof(sock.ipv6Addr));
			return ;
		}
		if (fcntl(sock.ipv6, F_SETFL, flags | O_NONBLOCK) < 0) {
			std::cout << "ipv6 socket cant be set to nonblocking" << std::endl;
			sock.ipv6 = 0;
			memset((void *)&sock.ipv6Addr, 0, sizeof(sock.ipv6Addr));
			return ;
		}
		std::cout << "ipv6 socket is listening on port: " << port << std::endl;
	}
	if (sock.local > 0) {
		if (listen(sock.local, 10) < 0) {
			std::cout << "listen() failed for local socket" << std::endl;
			sock.local = 0;
			memset((void *)&sock.localAddr, 0, sizeof(sock.localAddr));
			return ;
		}
		int flags = fcntl(sock.local, F_GETFL, 0);
		if (flags < 0) {
			std::cout << "fcntl() failed for local socket" << std::endl;
			sock.local = 0;
			memset((void *)&sock.localAddr, 0, sizeof(sock.localAddr));
			return ;
		}
		if (fcntl(sock.local, F_SETFL, flags | O_NONBLOCK) < 0) {
			std::cout << "local socket cant be set to nonblocking" << std::endl;
			sock.local = 0;
			memset((void *)&sock.localAddr, 0, sizeof(sock.localAddr));
			return ;
		}
		std::cout << "localhost socket is listening on port: " << port << std::endl;
	}
}

// void	socketManager::start(void (*networkInterface(int fd, const std::string &appId))) {
// 	while(true) {
		
// 	}
// }
