/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tintin_reporter.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/17 14:03:20 by kiroussa          #+#    #+#             */
/*   Updated: 2025/07/17 19:01:42 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#if MATT_MODE
#include "Tintin_reporter.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <ctime>
#include <sstream>
#include <sys/stat.h>

Tintin_reporter::Tintin_reporter()
{
	this->fd = -1;
}

Tintin_reporter::~Tintin_reporter()
{
	if (this->fd != -1)
		close(this->fd);
}

int Tintin_reporter::init(std::string const& parent_dir, std::string const& path)
{
#if SHIELD_DEBUG
	(void) parent_dir, (void) path;
	this->fd = 1;
	info("SHIELD_DEBUG enabled, logging to stdout");
#else // !SHIELD_DEBUG
	if (mkdir(parent_dir.c_str(), 0755) == -1)
	{
		if (errno != EEXIST)
			return 1;
	}
	
	this->fd = open(path.c_str(), O_WRONLY | O_APPEND);
	if (this->fd == -1)
		return 1;
#endif // !SHIELD_DEBUG
	info("Started");
	return 0;
}

void Tintin_reporter::report(std::string const& type, std::string const& message)
{
	std::stringstream ss;

	{
		time_t timestamp = time(NULL);
		struct tm* now = localtime(&timestamp);
		char time_str[64];
		strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", now);
		ss << "[" << time_str << "]";
	}
	ss << " [ " << type << " ]";
	ss << " - MattDaemon: ";
	ss << message;
	if (message.back() != '\n')
		ss << ".\n";

	std::string result = ss.str();
	(void)!write(this->fd, result.c_str(), result.size());
}

void Tintin_reporter::info(std::string const& message)
{
	this->report("INFO", message);
}

void Tintin_reporter::error(std::string const& message)
{
	this->report("ERROR", message);
}
#endif // MATT_MODE
