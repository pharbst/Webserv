/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socketManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/24 18:31:42 by pharbst           #+#    #+#             */
/*   Updated: 2023/12/25 00:31:39 by pharbst          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKETMANAGER_HPP
# define SOCKETMANAGER_HPP

# include <iostream>
# include <sys/socket.h>
# include <map>

# define TCP SOCK_STREAM
# define UDP SOCK_DGRAM
# define LOCALHOST AF_LOCAL
# define IPV4 AF_INET
# define IPV6 AF_INET6
# define IP 0

typedef struct s_socket {
	int				ipv4;
	int				ipv6;
	sockaddr_in		ipv4Addr;
	sockaddr_in6	ipv6Addr;
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
		void	createSocket(uint32_t port, uint8_t ipVersion, uint8_t protocol);

	private:
		// private attributes
		static int						_socketLocal;
		// port, sockets
		static std::map<int, t_socket>	_sockets;
		// port, socket
		static std::map<int, int>		_clientSockets;
};

#endif