/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InterfaceTools.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 12:02:48 by pharbst           #+#    #+#             */
/*   Updated: 2024/01/21 16:53:21 by pharbst          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Interface.hpp"

bool	Interface::readFromSocket(int sock, std::string &request) {
	int	n = BUFFER_SIZE;
	char buffer[BUFFER_SIZE];
	while (n == BUFFER_SIZE) {
		n = read(sock, buffer, BUFFER_SIZE);
		if (n < 0) {
			std::cout << "recv failed" << std::endl;
			return (true);
		}
		if (n == 0 && request.empty()) {
			std::cout << "client disconnected" << std::endl;
			return (true);
		}
		request.append(buffer, n);
	}
	request.append(buffer, n);
	return (false);
}

bool	Interface::passRequest(std::string &request, std::string &response, uint32_t port) {
	if (request.empty())
		return (true);
	if (_protocolMap.find(port) != _protocolMap.end()) {
		response = _protocolMap[port](request);
		return (false);
	}
	// for (std::map<int, protocolFunction>::iterator it = _protocolMap.begin(); it != _protocolMap.end(); it++) {
	// 	if () {
	// 		std::cout << "protocol found" << std::endl;
	// 		response = _protocolMap[it->first](request);
	// 		return (false);
	// 	}
	// }
	std::cout << "request: " << request << std::endl;
	std::cout << "unknown protocol" << std::endl;
	return (true);
}

bool	Interface::writeToSocket(int sock, std::string &response) {
	int i = send(sock, response.c_str(), response.length(), 0);
	if (i < 0 || i == 0)
		return (true);
	return (false);
}

#if defined(__SSL__)
bool	Interface::sslReadFromSocket(int sock, std::string &request, t_data sockData) {
	(void)sock;
	int	n = BUFFER_SIZE;
	char buffer[BUFFER_SIZE];
	while (n == BUFFER_SIZE) {
		n = SSL_read(sockData.sslSession, buffer, BUFFER_SIZE);
		if (n < 0) {
			std::cout << "recv failed" << std::endl;
			return (true);
		}
		if (n == 0 && request.empty()) {
			std::cout << "client disconnected" << std::endl;
			return (true);
		}
		request.append(buffer, n);
	}
	request.append(buffer, n);
	return (false);
}

bool	Interface::sslWriteToSocket(int sock, std::string &response, t_data sockData) {
	(void)sock;
	int i = SSL_write(sockData.sslSession, response.c_str(), response.length());
	if (i < 0 || i == 0)
		return (true);
	return (false);
}
#endif
