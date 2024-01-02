/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socketManagerTools.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/30 15:12:07 by pharbst           #+#    #+#             */
/*   Updated: 2024/01/02 17:15:51 by pharbst          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socketManager.hpp"

uint16_t	socketManager::validateCreationParams(const uint32_t port, uint32_t ipVersion) {
	// validate port
	{
		if (port > 0XFFFF) {
			std::cout << "Port is out of range" << std::endl;
			return 1;
		}
	}
	// validate ipVersion
	uint16_t		protocols = 0;
	{
		if (ipVersion == 0) {
			std::cout << "No ipVersion specified" << std::endl;
			return 1;
		}
		if (ipVersion / IPV6 == 1) {
			ipVersion -= IPV6;
			protocols += 1 << 15;
		}
		if (ipVersion / IPV4 == 1) {
			ipVersion -= IPV4;
			protocols += 1 << 14;
		}
		if (ipVersion / LOCALHOST == 1) {
			ipVersion -= LOCALHOST;
			protocols += 1 << 13;
		}
		if (ipVersion != 0) {
			std::cout << "Invalid ipVersion or combination of ipVersons" << std::endl;
			return 1;
		}
	}
	// validate uniqueness of port
	{
		std::map<uint32_t, t_socket>::iterator it = _sockets.find(port);
		if (it != _sockets.end()) {
			std::cout << "Sockets for port " << port << " already exist" << std::endl;
			std::cout << "Recreating sockets for port " << port << std::endl;
			socketManager::deleteSockets(port);
		}
	}
	return protocols;
}

void	socketManager::deleteSockets(const uint32_t port) {
	std::map<uint32_t, t_socket>::iterator it = _sockets.find(port);
	if (it == _sockets.end())
		return ;
	if (it->second.ipv4)
		close(it->second.ipv4);
	if (it->second.ipv6)
		close(it->second.ipv6);
	if (it->second.local)
		close(it->second.local);
	_sockets.erase(it);
}

t_socket	socketManager::createSockets(const uint16_t interface) {
	t_socket sock;
	std::memset(&sock, 0, sizeof(sock));
	if ((interface >> 15) % 2)
		sock.ipv6 = socket(IPV6, TCP, 0);
	if ((interface >> 14) % 2)
		sock.ipv4 = socket(IPV4, TCP, 0);
	if ((interface >> 13) % 2)
		sock.local = socket(LOCALHOST, TCP, 0);
	return sock;
}

uint32_t	socketManager::checkSocketCreation(t_socket &sock, const uint16_t interface, const uint32_t port) {
	uint32_t		socketNum = 0;
	// info print and counting sockets
	{
		if (sock.ipv6 == -1) {
			std::cout << "Failed to create socket for ipv6 on port: " << port << std::endl;
			sock.ipv6 = 0;
		}
		else if ((interface >> 15) % 2)
			socketNum++;
		if (sock.ipv4 == -1) {
			std::cout << "Failed to create socket for ipv4 on port: " << port << std::endl;
			sock.ipv4 = 0;
		}
		else if ((interface >> 14) % 2)
			socketNum++;
		if (sock.local == -1) {
			std::cout << "Failed to create socket for localhost on port: " << port << std::endl;
			sock.local = 0;
		}
		else if ((interface >> 13) % 2)
			socketNum++;
	}
	return socketNum;
}

void	socketManager::setReuseAddr(t_socket &sock) {
	int optval = 1;
	if (sock.ipv4 > 0)
		if (setsockopt(sock.ipv4, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)))
			throw socketManager::reuseAddressException();
	if (sock.ipv6 > 0)
		if (setsockopt(sock.ipv6, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)))
			throw socketManager::reuseAddressException();
	if (sock.local > 0)
		if (setsockopt(sock.local, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)))
			throw socketManager::reuseAddressException();
}

void	socketManager::bindSockets(t_socket &sock, uint32_t port) {
	if (sock.ipv4 > 0) {
		sock.ipv4Addr.sin_family = IPV4;
		sock.ipv4Addr.sin_port = htons(port);
		sock.ipv4Addr.sin_addr.s_addr = INADDR_ANY;
		if (bind(sock.ipv4, (sockaddr*)&sock.ipv4Addr, sizeof(sock.ipv4Addr)) < 0) {
			std::cout << "Failed to bind socket for ipv4 on port: " << port << std::endl;
			close(sock.ipv4);
			memset(&sock.ipv4Addr, 0, sizeof(sock.ipv4Addr));
			sock.ipv4 = 0;
		}
	}
	if (sock.ipv6 > 0) {
		sock.ipv6Addr.sin6_family = IPV6;
		sock.ipv6Addr.sin6_port = htons(port);
		sock.ipv6Addr.sin6_addr = in6addr_any;
		if (bind(sock.ipv6, (sockaddr*)&sock.ipv6Addr, sizeof(sock.ipv6Addr)) < 0) {
			std::cout << "Failed to bind socket for ipv6 on port: " << port << std::endl;
			close(sock.ipv6);
			memset(&sock.ipv6Addr, 0, sizeof(sock.ipv6Addr));
			sock.ipv6 = 0;
		}
	}
	if (sock.local > 0) {
		sock.localAddr.sin_family = LOCALHOST;
		sock.localAddr.sin_port = htons(port);
		if (bind(sock.local, (sockaddr*)&sock.localAddr, sizeof(sock.localAddr)) < 0) {
			std::cout << "Failed to bind socket for localhost on port: " << port << std::endl;
			close(sock.local);
			memset(&sock.localAddr, 0, sizeof(sock.localAddr));
			sock.local = 0;
		}
	}
}
