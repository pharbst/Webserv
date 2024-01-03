/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socketManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/30 15:05:15 by pharbst           #+#    #+#             */
/*   Updated: 2024/01/03 19:23:51 by pharbst          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NEW_SOCKETMANAGER_HPP
# define NEW_SOCKETMANAGER_HPP

# include <iostream>
# include <sys/socket.h>
# include <netinet/in.h>
# include <map>
# include <cstring>
# include <unistd.h>
# include <fcntl.h>

# define TCP		SOCK_STREAM
# define UDP		SOCK_DGRAM
# define LOCALHOST	AF_LOCAL
# define IPV4		AF_INET
# define IPV6		AF_INET6
# define IP			0

typedef struct s_socket {
	int				ipv4;
	int				ipv6;
	int				local;
	sockaddr_in		ipv4Addr;
	sockaddr_in6	ipv6Addr;
	sockaddr_in		localAddr;
}	t_socket;

class socketManager {
	public:
		// public methods
		static void		createSocket(uint32_t port, uint32_t ipVersion, uint32_t protocol);
		// static void		start(void (*networkInterface(int fd, const std::string &appId)));
	private:
		// private methods
			// for createSocket public method
			static void			listenSocket(t_socket &sock, const uint32_t port);
			static uint16_t		validateCreationParams(const uint32_t port, uint32_t ipVersion);
			static void			deleteSockets(const uint32_t port);
			static t_socket		createSockets(const uint16_t interface);
			static uint32_t		checkSocketCreation(t_socket &sock, const uint16_t interface, const uint32_t port);
			static void			setReuseAddr(t_socket &sock);
			static void			bindSockets(t_socket &sock, uint32_t port);

			class reuseAddressException : public std::exception {
				public:
					virtual const char* what() const throw() {
						return "setsockopt(SO_REUSEADDR) failed";
					}
			};
			// for listenSocket public method
		// private attributes
		// port -> sockets
		static std::map<uint32_t, t_socket>		_sockets;
		// socket fd -> port
		// static std::map<int, uint32_t>		_clientSockets;
};

#endif