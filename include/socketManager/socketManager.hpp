/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socketManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/30 15:05:15 by pharbst           #+#    #+#             */
/*   Updated: 2024/01/21 16:28:42 by pharbst          ###   ########.fr       */
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

#if defined(__SSL__)
# include <openssl/ssl.h>
# include <openssl/err.h>
#endif

#if defined(__LINUX__) || defined(__linux__)
# include <sys/epoll.h>
# define SEPOLLQU socketManager::socketEpoll
# define SEPOLLQUREMOVE socketManager::epollRemove
#elif defined(__APPLE__)
# include <sys/event.h>
# define SEPOLLQU socketManager::socketKqueue
# define SEPOLLQUREMOVE socketManager::kqueueRemove
#else
# include <sys/select.h>
# define SEPOLLQU socketManager::socketSelect
# define SEPOLLQUREMOVE socketManager::selectRemove
#endif

# define TCP		SOCK_STREAM
# define UDP		SOCK_DGRAM
# define LOCALHOST	AF_LOCAL
# define IPV4		AF_INET
# define IPV6		AF_INET6
# define IP			0

typedef struct s_data {
	const std::string	interfaceAddress;
	uint32_t			port;
	uint32_t			protocol;
	#if defined(__SSL__)
	SSL_CTX				*ctx;
	SSL					*sslSession;
	bool				ssl;
	#endif
	bool				read;
	bool				write;
	bool				server;
}	t_data;

typedef struct s_socket {
	uint32_t			port;
	uint32_t			protocol;
	uint32_t			ipVersion;
	std::string			interfaceAddress;
	#if defined(__SSL__)
	std::string			certFile;
	std::string			keyFile;
	bool				ssl;
	#endif
}	t_socket;

typedef void	(*InterfaceFunction)(int sock, t_data sockData);

class socketManager {
	public:
		static void							addSocket(const t_socket &socket);
		static void							removeSocket(int fd);
		static void							printMap();
		static void							start(InterfaceFunction interfaceFunction);
	private:
		static std::map<int, t_data>		_sockets;

	#if defined(__LINUX__) || defined(__linux__)
		static int							_epollfd;
	#elif defined(__APPLE__)
		static int							_kq;
		static struct kevent				_changes[2];
		static struct kevent				_events[2];
	#else
		static fd_set						_interest;
		static int							_maxfd;
	#endif

		static bool							bindSocket(int fd, const std::string &interfaceAddress, uint32_t port, uint32_t ipVersion);
		static bool							validateCreationParams(const std::string &interfaceAddress, uint32_t port, uint32_t protocol);
	#if defined(__SSL__)
		static void							sslInit(const t_socket &newSocket, int fd);
		static void							sslAccept(int newClient, int fd);
	#endif
	#if defined(__LINUX__) || defined(__linux__)
		static void							socketEpoll(InterfaceFunction interfaceFunction);
		static void							epollRemove(int fd);
		static void							epollAccept(int fd);
	#elif defined(__APPLE__)
		static void							socketKqueue(InterfaceFunction interfaceFunction);
		static void							kqueueRemove(int fd);
		static void							kqueueAccept(int fd);
	#else
		static void							socketSelect(InterfaceFunction interfaceFunction);
		static void							selectRemove(int fd);
		static void							selectAccept(int fd);
	#endif
};

#endif