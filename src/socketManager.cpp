/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socketManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/30 15:05:28 by pharbst           #+#    #+#             */
/*   Updated: 2024/01/02 17:15:57 by pharbst          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socketManager.hpp"

std::map<uint32_t, t_socket>	socketManager::_sockets;
std::map<uint32_t, t_socket>	socketManager::_clientSockets;

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
	// add node to _sockets
	_sockets.insert(std::pair<uint32_t, t_socket>(port, sock));
}

void	socketManager::listenSocket(const uint32_t port) {
	std::map<uint32_t, t_socket>::iterator it = _sockets.find(port);
	if (it == _sockets.end()) {
		std::cout << "No socket found for port: " << port << std::endl;
		return ;
	}
	if (it->second.ipv4 > 0) {
		if (listen(it->second.ipv4, 10) < 0) {
			std::cout << "listen() failed for ipv4 socket" << std::endl;
			return ;
		}
		std::cout << "ipv4 socket is listening on port: " << port << std::endl;
	}
	if (it->second.ipv6 > 0) {
		if (listen(it->second.ipv6, 10) < 0) {
			std::cout << "listen() failed for ipv6 socket" << std::endl;
			return ;
		}
		std::cout << "ipv6 socket is listening on port:" << port << std::endl;
	}
	if (it->second.local > 0) {
		if (listen(it->second.local, 10) < 0) {
			std::cout << "listen() failed for local socket" << std::endl;
			return ;
		}
		std::cout << "localhost socket is listening on port: " << port << std::endl;
	}
}
