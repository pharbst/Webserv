/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socketManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/30 15:05:28 by pharbst           #+#    #+#             */
/*   Updated: 2024/01/20 20:19:33 by pharbst          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socketManager.hpp"

std::map<int, t_data>		socketManager::_sockets;

void	socketManager::start(InterfaceFunction interfaceFunction) {
	#if defined(__SSL__)
	SSL_library_init();
	SSL_load_error_strings();
	#endif
	SEPOLLQU(interfaceFunction);
}

void	socketManager::removeSocket(int fd) {
	SEPOLLQUREMOVE(fd);
	close(fd);
	_sockets.erase(fd);
}

void	socketManager::addSocket(const t_socket &newSocket) {
	if (validateCreationParams(newSocket.interfaceAddress, newSocket.port, newSocket.protocol))
		return ;
	// create socket with error check
	int fd = socket(newSocket.ipVersion, newSocket.protocol, 0);
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
	if (bindSocket(fd, newSocket.interfaceAddress, newSocket.port, newSocket.ipVersion))
		return ;
	// listen on socket with error check
	{
		if (listen(fd, 10) == -1) {
			std::cout << "Socket listening failed" << std::endl;
			return ;
		}
	}
	#if defined(__SSL__)
	// init ssl context
	sslInit(newSocket, fd);
	#else
	// add socket to map
	t_data data = {newSocket.interfaceAddress, newSocket.port, newSocket.protocol, false, false, true};
	_sockets.insert(std::pair<int, t_data>(fd, data));
	#endif
}
