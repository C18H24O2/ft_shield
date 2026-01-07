/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tintin_reporter.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/17 14:03:20 by kiroussa          #+#    #+#             */
/*   Updated: 2026/01/07 12:01:00 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#if MATT_MODE
#define TINTIN_ARCHIVAL 1
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

	int pid = getpid();
	if (pid == -1)
	{
		DEBUG("Failed to get pid: %m\n");
		return 1;
	}
	std::stringstream ss;
	ss << parent_dir << "/" << name << ".log";
	std::string path = ss.str();

#if TINTIN_ARCHIVAL
	struct stat s;
	DEBUG("checking %s for archival\n", path.c_str());

	if (stat(path.c_str(), &s) == 0)
	{
		DEBUG("old found, moving\n");
		struct tm* tval = localtime(&s.st_mtime);
		tval->tm_year += 1900;
		tval->tm_mon += 1;
		
		std::string year_path = parent_dir + "/" + std::to_string(tval->tm_year);
		if (mkdir(year_path.c_str(), 0755) == -1)
		{
			if (errno != EEXIST)
				goto skip;
		}
		std::string month_path = year_path + "/" + std::to_string(tval->tm_mon);
		if (mkdir(month_path.c_str(), 0755) == -1)
		{
			if (errno != EEXIST)
				goto skip;
		}

		std::string timestamp_part = std::to_string(tval->tm_year) + "-" + std::to_string(tval->tm_mon) + "-" + std::to_string(tval->tm_mday)
			+ "_" + std::to_string(tval->tm_hour) + "-" + std::to_string(tval->tm_min) + "-" + std::to_string(tval->tm_sec);

		std::string new_path = month_path + "/" + timestamp_part + ".log";

		int i = 1;
		while (access(new_path.c_str(), F_OK) == 0)
		{
			new_path = month_path + "/" + timestamp_part + "-" + std::to_string(i) + ".log";
			i++;
		}

		DEBUG("renaming %s to %s\n", path.c_str(), new_path.c_str());
		if (rename(path.c_str(), new_path.c_str()) == -1)
			goto skip;
		DEBUG("done renaming\n");

		path = new_path;

		DEBUG("checking for tar\n");
		if (shield_path_check("tar"))
		{
			std::string tar_path = month_path + "/" + timestamp_part + ".tar.gz";
			i = 1;
			while (access(tar_path.c_str(), F_OK) == 0)
			{
				tar_path = month_path + "/" + timestamp_part + "-" + std::to_string(i) + ".tar.gz";
				i++;
			}
			DEBUG("tar found, compressing %s to %s\n", path.c_str(), tar_path.c_str());
			std::string cmd = "tar -czf \'" + tar_path + "\' \'" + path + "\' >/dev/null 2>&1";
			DEBUG("running %s\n", cmd.c_str());
			if (system(cmd.c_str()) == -1)
				goto skip;
			DEBUG("success, removing old logfile %s\n", path.c_str());
			unlink(path.c_str());
			path = tar_path;
		}

		goto end;
	}
	skip:
	{
		unlink(path.c_str());
	}
	end:
#endif

	path = ss.str();
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
