/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   applicationInterface.hpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pharbst <pharbst@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 17:34:45 by pharbst           #+#    #+#             */
/*   Updated: 2024/01/10 19:39:47 by pharbst          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef APPLICATIONINTERFACE_HPP
# define APPLICATIONINTERFACE_HPP

# include <iostream>
# include <map>
# include </sys/select.h>

# define BUFFER_SIZE 1024

typedef const std::string &(*appFunction)(const std::string &);

typedef struct s_info {
	int					socket;
	uint32_t			port;
}	t_info;

class applicationInterface {
	public:
		static void						addApplication(uint32_t port, const std::string (&application(const std::string &request)));
		static void						Interface(int pipeRead);

	private:
		static void						waitOnPipe(int pipe);
		static std::string				readLine(int pipe);
		static bool						parsePipe(const std::sring &line);

		static std::map<uint32_t, appFunction>	_applications;
};

#endif