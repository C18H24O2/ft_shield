/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/17 01:18:29 by kiroussa          #+#    #+#             */
/*   Updated: 2025/07/17 02:04:20 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <shield/daemon.h>
#include "DaemonServer.hpp"
#include "shield.h"
#include <cstring>

void	shield_daemon_run(void)
{
	char	password_hash[32];
	memset(password_hash, 0, 32);
	DaemonServer server(password_hash);

	if (server.init())
	{
		DEBUG("Failed to initialize the server\n");
		return ;
	}
	server.run();
}
