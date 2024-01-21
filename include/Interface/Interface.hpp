/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Interface.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 17:34:45 by pharbst           #+#    #+#             */
/*   Updated: 2024/01/21 16:42:59 by pharbst          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef APPLICATIONINTERFACE_HPP
# define APPLICATIONINTERFACE_HPP

# include "socketManager.hpp"
# include <cstdlib>

# define BUFFER_SIZE 1024

typedef std::string (*protocolFunction)(const std::string &request);

class Interface {
	public:
		static void						addProtocol(uint32_t port, protocolFunction function);
		static void						interface(int sock, t_data sockData);

	private:
		#if defined(__SSL__)
		static bool						sslReadFromSocket(int sock, std::string &request, t_data sockData);
		static bool						sslWriteToSocket(int sock, std::string &response, t_data sockData);
		#endif
		static bool						readFromSocket(int sock, std::string &request);
		static bool						passRequest(std::string &request, std::string &response, uint32_t port);
		static bool						writeToSocket(int sock, std::string &response);

		static std::map<uint32_t, protocolFunction>			_protocolMap;
		static std::map<int, std::string>				_outputBuffer;
};

#endif