/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   new_socketManager.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/30 15:05:15 by pharbst           #+#    #+#             */
/*   Updated: 2024/01/02 16:50:53 by pharbst          ###   ########.fr       */
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
		static void		listenSocket(const uint32_t port);
	private:
		// private methods
			// for createSocket public method
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
		static std::map<uint32_t, t_socket>		_sockets;
		static std::map<uint32_t, t_socket>		_clientSockets;
};

#endif