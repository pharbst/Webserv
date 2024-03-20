/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InterfaceTools.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 12:02:48 by pharbst           #+#    #+#             */
/*   Updated: 2024/03/20 19:52:25 by pharbst          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Interface.hpp"

int	Interface::readFromSocket(int sock, struct sockData data, std::string &request) {
	int	n = BUFFER_SIZE;
	char buffer[BUFFER_SIZE];
	while (n == BUFFER_SIZE) {
		if (data.info.ssl)
			n = SSL_read((SSL*)data.info.sslData.Context, buffer, BUFFER_SIZE);
		else
			n = recv(sock, buffer, BUFFER_SIZE, 0);
		if (n < 0) {
			std::cout << "recv failed" << std::endl;
			return n;
		}
		if (n == 0 && request.empty())
			return n;
		request.append(buffer, n);
	}
	return 1;
}

bool	Interface::passRequest(std::string &request, std::string &response, uint32_t port) {
	for (std::map<uint32_t, http*>::iterator it = _executerMap.begin(); it != _executerMap.end(); it++) {
		if (_executerMap.find(port) != _executerMap.end())
			response = _executerMap[port]->executer(request);
		return (false);
	}
	return (false);
}

bool	Interface::writeToSocket(int sock, struct sockData data, std::string &response) {
	int i;
	if (response.empty())
		return (false);
	if (data.info.ssl)
		i = SSL_write((SSL*)data.info.sslData.Context, response.c_str(), response.length());
	else
		i = send(sock, response.c_str(), response.length(), 0);
	if (i < 0)
		return (true);
	return (false);
}