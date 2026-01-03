/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run.cc                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/17 01:18:29 by kiroussa          #+#    #+#             */
/*   Updated: 2026/01/04 00:45:19 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <shield/server.h>

#if MATT_MODE
#define LOG_BUT_ONLY_IF_MATT_OTHERWISE_DEBUG(x) printf("%s", x)
#else
#define LOG_BUT_ONLY_IF_MATT_OTHERWISE_DEBUG(x) DEBUG(x)
#endif

void	shield_daemon_run(void)
{
	daemon_server_t server;

	if (server_init(&server))
	{
		LOG_BUT_ONLY_IF_MATT_OTHERWISE_DEBUG("Failed to initialize the server.\n");
		return ;
	}
	server_run(&server);
	server_cleanup(&server);
}
