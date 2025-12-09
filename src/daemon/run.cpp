/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/17 01:18:29 by kiroussa          #+#    #+#             */
/*   Updated: 2025/12/09 21:38:44 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <shield/daemon.h>
#include "DaemonServer.hpp"

#if MATT_MODE
#define LOG_BUT_ONLY_IF_MATT_OTHERWISE_DEBUG(x) std::cout << x
#else
#define LOG_BUT_ONLY_IF_MATT_OTHERWISE_DEBUG(x) DEBUG(x)
#endif

void	shield_daemon_run(void)
{
	DaemonServer server;

	if (server.init())
	{
		LOG_BUT_ONLY_IF_MATT_OTHERWISE_DEBUG("Failed to initialize the server.\n");
		return ;
	}
	server.run();
}
