/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socketManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/24 18:31:42 by pharbst           #+#    #+#             */
/*   Updated: 2023/12/25 20:48:25 by pharbst          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKETMANAGER_HPP
# define SOCKETMANAGER_HPP

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

/**
 * @brief Class that manages all sockets
 * @param 
 */
class socketManager {
	public:
		// public methods
		/**
		 * @brief Create a Socket object
		 * 
		 * @param port port to listen on
		 * @param ipVersion IPV4, IPV6 or LOCALHOST (to use both ip versions use IPV4 + IPV6)
		 * @param protocol protocol to use for client sockets (TCP or UDP) the listening socket will always use TCP
		 */
		static void	createSocket(uint32_t port, uint8_t ipVersion, uint8_t protocol);
		static void	startListening(uint32_t port);
		static const std::map<int, t_socket>::iterator getSockets(int port);

	private:
		// private methods
		static int	bindSocket(int socket, struct sockaddr* addr);
		static int addSocket(uint8_t ipVersion, uint32_t port, t_socket &sock);
		// port, sockets
		static std::map<int, t_socket>		_sockets;
		// origin port, socket
		static std::map<int, int>			_clientSockets;
};

#endif