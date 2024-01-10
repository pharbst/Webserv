/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   applicationInterfaceTools.cpp                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 18:27:08 by pharbst           #+#    #+#             */
/*   Updated: 2024/01/10 19:45:54 by pharbst          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "applicationInterface.hpp"

void	applicationInterface::waitOnPipe(int pipe) {
	fd_set			readfds;
	FD_ZERO(&readfds);
	FD_SET(pipe, &readfds);
	if (select(pipe + 1, &readfds, NULL, NULL, NULL) == -1) {
		std::cout << "Error: Select failed" << std::endl;
		return ;
	}
}

t_info	applicationInterface::parsePipe(const std::string &line) {
	t_info info;
	std::string::size_type pos = line.find(' ');
	if (pos == std::string::npos) {
		std::cout << "Error: Invalid pipe format" << std::endl;
		info.socket = -1;
		return (info);
	}
	info.socket = std::stoi(line.substr(0, pos));
	info.port = std::stoi(line.substr(pos + 1));
	return (info);
}

std::string applicationInterface::readLine(int fd) {
	static char		stash[BUFFER_SIZE];
	if (fd < 0 || fd > OPEN_MAX)
		return (NULL);
	std::string line;
	{
		int n = BUFFER_SIZE;
		while (n == BUFFER_SIZE && line.find('\n') == std::string::npos) {
			line += stash;
			n = read(fd, stash, BUFFER_SIZE);
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
		memmove(stash, pos + 1, BUFFER_SIZE - (pos - stash));
	}
	return (line);
}
