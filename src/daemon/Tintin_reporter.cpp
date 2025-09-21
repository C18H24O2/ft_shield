/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tintin_reporter.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/17 14:03:20 by kiroussa          #+#    #+#             */
/*   Updated: 2025/09/21 17:02:09 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#if MATT_MODE
#include <shield/le_function.h>
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

int Tintin_reporter::init(std::string const& parent_dir, std::string const& name)
{
#if SHIELD_DEBUG
	char* env = getenv("SHIELD_LOG_STDOUT");
	if (env) {
		this->fd = 1;
		info("SHIELD_DEBUG enabled and SHIELD_LOG_STDOUT var set, logging to stdout");
		return 0;
	}
#endif // SHIELD_DEBUG
	DEBUG("Tintin_reporter::init(\"%s\", \"%s\")\n", parent_dir.c_str(), name.c_str());

	if (mkdir(parent_dir.c_str(), 0755) == -1)
	{
		if (errno != EEXIST)
		{
			DEBUG("Failed to create the directory \"%s\": %m\n", parent_dir.c_str());
			return 1;
		}
	}

	int pid = le_getpid();
	if (pid == -1)
	{
		DEBUG("Failed to get pid: %m\n");
		return 1;
	}
	std::stringstream ss;
	ss << parent_dir << "/" << name << ".log";
	std::string path = ss.str();
	DEBUG("creating logfile: %s\n", path.c_str());
	
	this->fd = open(path.c_str(), O_WRONLY | O_APPEND | O_CREAT, 0644);
	if (this->fd == -1)
	{
		DEBUG("Failed to open the file \"%s\": %m\n", path.c_str());
		return 1;
	}
	info("Started.");
	return 0;
}

void Tintin_reporter::report(std::string const& type, std::string const& message)
{
	std::stringstream ss;

	{
		time_t timestamp = time(NULL);
		struct tm* now = localtime(&timestamp);
		char time_str[64];
		strftime(time_str, sizeof(time_str), "%d / %m / %Y - %H : %M : %S", now);
		ss << "[ " << time_str << "]";
	}
	ss << " [ " << type << " ]";
	ss << " - Matt_daemon: ";
	ss << message;
	if (message.back() != '\n')
		ss << "\n";

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
