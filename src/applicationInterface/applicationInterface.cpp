/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   applicationInterface.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 17:37:00 by pharbst           #+#    #+#             */
/*   Updated: 2024/01/10 22:37:30 by pharbst          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "applicationInterface.hpp"

std::map<uint32_t, appFunction>		applicationInterface::_applications;

void	applicationInterface::Interface(int pipeRead) {
	while (true) {
		// wait for pipe to contain a whole line
		waitOnPipe(pipeRead);
		while (!(std::string line = readLine(pipeRead)).empty()) {
			parsePipe(line);
			if (info.socket == -1)
				continue ;
			// call application
			std::string response = _applications[info.port](readFromSocket(info.socket));
			// write response to socket
			write(info.socket, response.c_str(), response.length());
			// send signal to main process
			kill(getppid(), SIGUSR1);
			// start over
		}
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