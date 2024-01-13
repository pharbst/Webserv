/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   applicationInterfaceTools.cpp                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 18:27:08 by pharbst           #+#    #+#             */
/*   Updated: 2024/01/11 17:45:39 by pharbst          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "applicationInterface.hpp"

void	applicationInterface::waitOnPipe(int pipe, fd_set readfds) {
	if (select(pipe + 1, &readfds, NULL, NULL, NULL) == -1) {
		std::cout << "Error: Select failed" << std::endl;
		return ;
	}
}

t_info	applicationInterface::parsePipe(const std::string &line) {
	t_info info;
	std::istringstream iss(line);
	if (!(iss >> info.socket >> info.port)) {
		std::cout << "Error: Pipe parse failed" << std::endl;
		info.socket = -1;
	}
	return (info);
}

std::string	applicationInterface::readFromSocket(int socket) {
	std::string	request;
	char		buffer[BUFFER_SIZE];
	int			n;
	while ((n = read(socket, buffer, BUFFER_SIZE - 1)) == BUFFER_SIZE - 1) {
		buffer[n] = '\0';
		request += buffer;
	}
	buffer[n] = '\0';
	request += buffer;
	return (request);
}

std::string applicationInterface::readLine(int fd) {
	static char		stash[BUFFER_SIZE];
	std::string line;
	{
		int n = BUFFER_SIZE - 1;
		while (n == BUFFER_SIZE - 1 && line.find('\n') == std::string::npos) {
			line += stash;
			n = read(fd, stash, BUFFER_SIZE - 1);
			if (n == -1) {
				std::cout << "Error: Read failed" << std::endl;
				return (NULL);
			}
			stash[n] = '\0';
		}
	}
	{
		char *pos = strchr(stash, '\n');
		if (!pos)
			return (line += stash, stash[0] = '\0', line);
		line += std::string(stash, pos - stash);
		memmove(stash, pos + 1, BUFFER_SIZE - (pos - stash + 1));
	}
	return (line);
}
