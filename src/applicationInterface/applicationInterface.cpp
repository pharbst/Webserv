/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   applicationInterface.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 17:37:00 by pharbst           #+#    #+#             */
/*   Updated: 2024/01/11 17:24:07 by pharbst          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "applicationInterface.hpp"

std::map<uint32_t, appFunction>		applicationInterface::_applications;

void	applicationInterface::Interface(int pipeRead) {
	fd_set			readfds;
	FD_ZERO(&readfds);
	FD_SET(pipeRead, &readfds);
	while (true) {
		waitOnPipe(pipeRead, readfds);
		std::string line = readLine(pipeRead);
		t_info info = parsePipe(line);
		if (info.socket == -1)
			continue ;
		std::string response = _applications[info.port](readFromSocket(info.socket));
		write(info.socket, response.c_str(), response.length());
		kill(getppid(), SIGUSR1);
	}
}

void	applicationInterface::addApplication(uint32_t port, appFunction application) {
	if (port > 0XFFFF) {
		std::cout << "Error: Port is too big" << std::endl;
		return ;
	}
	if (_applications[port]) {
		std::cout << "Error: Port is already in use" << std::endl;
		return ;
	}
	_applications.insert(std::pair<uint32_t, appFunction>(port, application));
}