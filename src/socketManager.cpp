/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socketManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/30 15:05:28 by pharbst           #+#    #+#             */
/*   Updated: 2024/01/07 19:13:43 by pharbst          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socketManager.hpp"

std::map<int, t_data>		socketManager::_sockets;

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
	// set socketopt to reuse address with error check
	{
		int opt = 1;
		if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
			std::cout << "Socketopt setting failed" << std::endl;
			return ;
		}
	}
	// bind socket with error check
	{
		if (ipVersion == IPV4) {
			sockaddr_in addr;
			addr.sin_family = AF_INET;
			addr.sin_port = htons(port);
			if (inet_pton(AF_INET, interfaceAddress.c_str(), &addr.sin_addr) != 1) {
				std::cout << "Invalid interface address" << std::endl;
				return ;
			}
			if (bind(fd, (sockaddr *)&addr, sizeof(addr)) == -1) {
				std::cout << "Socket binding failed" << std::endl;
				return ;
			}
		}
		else if (ipVersion == IPV6) {
			sockaddr_in6 addr;
			addr.sin6_family = AF_INET6;
			addr.sin6_port = htons(port);
			if (inet_pton(AF_INET6, interfaceAddress.c_str(), &addr.sin6_addr) != 1) {
				std::cout << "Invalid interface address" << std::endl;
				return ;
			}
			if (bind(fd, (sockaddr *)&addr, sizeof(addr)) == -1) {
				std::cout << "Socket binding failed" << std::endl;
				return ;
			}
		}
		else {
			std::cout << "Invalid ipVersion" << std::endl;
			return ;
		}
	}
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

// void	socketManager::start(void (*networkInterface(int fd, const std::string &appId))) {
// 	// create fd_set
// 	// add all sockets to fd_set
// 	// select on fd_set
// 	// check if fd is in fd_set
// 	// call networkInterface
// }
