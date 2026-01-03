/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/17 01:18:29 by kiroussa          #+#    #+#             */
/*   Updated: 2026/01/03 19:41:04 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <shield/daemon.h>
#include "DaemonServer.hpp"

#if MATT_MODE
#define LOG_BUT_ONLY_IF_MATT_OTHERWISE_DEBUG(x) printf("%s", x)
#else
#define LOG_BUT_ONLY_IF_MATT_OTHERWISE_DEBUG(x) DEBUG(x)
#endif

void	shield_daemon_run(void)
{
	DaemonServer server;

	if (daemon_init(&server))
	{
		LOG_BUT_ONLY_IF_MATT_OTHERWISE_DEBUG("Failed to initialize the server.\n");
		return ;
	}
	daemon_run(&server);
	daemon_cleanup(&server);
}
