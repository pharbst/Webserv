/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socketManagerTools.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/30 15:12:07 by pharbst           #+#    #+#             */
/*   Updated: 2024/01/07 19:15:38 by pharbst          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socketManager.hpp"

bool	socketManager::validateCreationParams(const std::string &interfaceAddress, uint32_t port, uint32_t protocol) {
	// validate port
	{
		if (port > 0XFFFF) {
			std::cout << "Port is out of range" << std::endl;
			return true;
		}
	}
	// validate protocol
	{
		switch (protocol)
		{
		case TCP:
			break;
		case UDP:
			break;
		default:
			return true;
		}
	}
	// check if socket already exists
	{
		for (std::map<int, t_data>::iterator it = _sockets.begin(); it != _sockets.end(); it++) {
			if (it->second.port == port && it->second.protocol == protocol && it->second.interfaceAddress == interfaceAddress) {
				std::cout << "Socket already exists" << std::endl;
				return true;
			}
		}
	}
	return false;
}

// socketManager::socketEpoll() {
	
// }
