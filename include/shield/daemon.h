/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   daemon.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 14:20:31 by kiroussa          #+#    #+#             */
/*   Updated: 2025/03/29 19:24:40 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DAEMON_H
# define DAEMON_H

enum e_client_state
{
	CLIENT_STATE_CONNECTED = 0,
	CLIENT_STATE_AUTHENTICATED,
	CLIENT_STATE_SHELL,
};

typedef struct s_client
{
	int					fd;
	enum e_client_state	state;
}	t_client;

void	shield_daemonize(void);
void	shield_daemon_start(void);
int		shield_daemon_main(void);

#endif // DAEMON_H
