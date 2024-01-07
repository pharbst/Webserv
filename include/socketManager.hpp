/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socketManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/30 15:05:15 by pharbst           #+#    #+#             */
/*   Updated: 2024/01/07 19:13:00 by pharbst          ###   ########.fr       */
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

// #if defined(__LINUX__) || defined(__linux__)
// 	#include <sys/epoll.h>
// 	#define SEPOLL(use_epoll) socketEpoll(use_epoll)
// #else
// 	#define SEPOLL(use_select) socketSelect(use_select)
// #endif

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

class socketManager {
	public:
		static void	addSocket(const std::string &interfaceAddress, uint32_t port, uint32_t ipVersion, uint32_t protocol);
		// void	start(void (*networkInterface(int fd, const std::string &appId)));
	private:
		static bool	validateCreationParams(const std::string &interfaceAddress, uint32_t port, uint32_t protocol);
		static std::map<int, t_data>		_sockets;
};

#endif