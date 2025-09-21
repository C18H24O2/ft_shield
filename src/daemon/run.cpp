/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/17 01:18:29 by kiroussa          #+#    #+#             */
/*   Updated: 2025/09/21 16:43:58 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <shield/daemon.h>
#include "DaemonServer.hpp"
#include "shield.h"
#include <cstring>

#if MATT_MODE
#define LOG_BUT_ONLY_IF_MATT_OTHERWISE_DEBUG(x) std::cout << x
#else
#define LOG_BUT_ONLY_IF_MATT_OTHERWISE_DEBUG(x) DEBUG(x)
#endif

void	shield_daemon_run(void)
{
	char	password_hash[32];
	memset(password_hash, 0, 32);
	DaemonServer server(password_hash);

	if (server.init())
	{
		LOG_BUT_ONLY_IF_MATT_OTHERWISE_DEBUG("Failed to initialize the server.\n");
		return ;
	}
	server.run();
}
