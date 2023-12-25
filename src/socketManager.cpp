/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socketManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/25 16:49:04 by pharbst           #+#    #+#             */
/*   Updated: 2023/12/25 20:50:35 by pharbst          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "socketManager.hpp"

std::map<int, t_socket>			socketManager::_sockets;
std::map<int, int>				socketManager::_clientSockets;

void	socketManager::createSocket(uint32_t port, uint8_t ipVersion, uint8_t protocol) {
	(void)protocol;
	if (port > 0XFFFF) {
		std::cout << "Port is out of range" << std::endl;
		return ;
	}
	// maybe delete existing ports and recreate them
	if (getSockets(port) != _sockets.end()) {
		std::cout << "Port already in use" << std::endl;
		return ;
	}
	int	success = 0;
	t_socket sock;
	std::memset(&sock, 0, sizeof(sock));
	if (ipVersion / IPV6 == 1) {
		std::cout << "creating ipv6 socket" << std::endl;
		ipVersion -= IPV6;
		if (!addSocket(IPV6, port, sock))
			success++;
	}
	if (ipVersion / IPV4 == 1) {
		std::cout << "creating ipv4 socket" << std::endl;
		ipVersion -= IPV4;
		if (!addSocket(IPV4, port, sock))
			success++;
	}
	if (ipVersion / LOCALHOST == 1) {
		std::cout << "creating localhost socket" << std::endl;
		ipVersion -= LOCALHOST;
		if (!addSocket(LOCALHOST, port, sock))
			success++;
	}
	if (success == 0) {
		std::cout << "no socket created for port: " << port << std::endl;
		return ;
	}
	std::cout << port << "    " << success << " sockets created" << std::endl;
	std::cout << std::pair<int, t_socket>(port, sock).first << std::endl;
	_sockets.insert(std::pair<int, t_socket>(port, sock));
}

int	socketManager::addSocket(uint8_t ipVersion, uint32_t port, t_socket &sock) {
	switch (ipVersion)
	{
	case LOCALHOST:
		sock.local = socket(LOCALHOST, TCP, IP);
		if (sock.local < 0) {
			std::cout << "Failed to create socket for localhost on port: " << port << std::endl;
			return -1;
		}
		sock.localAddr.sin_family = LOCALHOST;
		sock.localAddr.sin_port = htons(port);
		sock.localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		if (bindSocket(sock.local, (struct sockaddr*)&sock.localAddr)) {
			close(sock.local);
			sock.local = 0;
			std::memset(&sock.localAddr, 0, sizeof(sock.localAddr));
			return -1;
		}
		return 0;
	case IPV4:
		sock.ipv4 = socket(IPV4, TCP, IP);
		if (sock.ipv4 < 0) {
			std::cout << "Failed to create socket for ipv4 on port: " << port << std::endl;
			return -1;
		}
		sock.ipv4Addr.sin_family = IPV4;
		sock.ipv4Addr.sin_port = htons(port);
		sock.ipv4Addr.sin_addr.s_addr = htonl(INADDR_ANY);
		if (bindSocket(sock.ipv4, (struct sockaddr*)&sock.ipv4Addr)) {
			close(sock.ipv4);
			sock.ipv4 = 0;
			std::memset(&sock.ipv4Addr, 0, sizeof(sock.ipv4Addr));
			return -1;
		}
		return 0;
	case IPV6:
		sock.ipv6 = socket(IPV6, TCP, IP);
		if (sock.ipv6 < 0) {
			std::cout << "Failed to create socket for ipv6 on port: " << port << std::endl;
			return -1;
		}
		sock.ipv6Addr.sin6_family = IPV6;
		sock.ipv6Addr.sin6_port = htons(port);
		sock.ipv6Addr.sin6_addr = in6addr_any;
		if (bindSocket(sock.ipv6, (struct sockaddr*)&sock.ipv6Addr)) {
			close(sock.ipv6);
			sock.ipv6 = 0;
			std::memset(&sock.ipv6Addr, 0, sizeof(sock.ipv6Addr));
			return -1;
		}
		return 0;
	default:
		std::cout << "Invalid ipVersion" << std::endl;
		return -1;
	}
}

int	socketManager::bindSocket(int socket, struct sockaddr* addr) {
	std::cout << "Binding socket" << std::endl;
	if (bind(socket, addr, sizeof(*addr)) < 0) {
		std::cout << "Failed to bind socket" << std::endl;
		return -1;
	}
	return 0;
}

const std::map<int, t_socket>::iterator socketManager::getSockets(int port) {
	std::map<int, t_socket>::iterator it = _sockets.find(port);
	return it;
}

void	socketManager::startListening(uint32_t port) {
	std::map<int, t_socket>::iterator it = getSockets(port);
	if (it == _sockets.end()) {
		std::cout << "No socket found for port: " << port << std::endl;
		return ;
	}
	unsigned int	success = 0;
	if (it->second.local > 0) {
		if (listen(it->second.local, 10) == -1)
			std::cout << "Listening for localhost socket failed on port: " << port << std::endl;
		else
			success++;
	}
	if (it->second.ipv4 > 0) {
		if (listen(it->second.ipv4, 10) == -1)
			std::cout << "Listening for ipv4 socket failed on port: " << port << std::endl;
		success += 2;
	}
	if (it->second.ipv6 > 0) {
		if (listen(it->second.ipv6, 10) == -1)
			std::cout << "Listening for ipv6 socket failed on port: " << port << std::endl;
		success += 4;
	}
	if (success == 0)
		std::cout << "No socket listening for port: " << port << std::endl;
	else {
		std::cout << "summary: " << std::endl;
		if (success >> 2 % 2) {
			std::cout << "ipv6 socket is listening on port: " << port << std::endl;
			success -= 4;
		}
		if (success >> 1 % 2) {
			std::cout << "ipv4 socket is listening on port: " << port << std::endl;
			success -= 2;
		}
		if (success % 2)
			std::cout << "localhost socket is listening on port: " << port << std::endl;
	}
}
