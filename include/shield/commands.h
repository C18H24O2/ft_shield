/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/28 22:00:44 by kiroussa          #+#    #+#             */
/*   Updated: 2026/01/04 00:45:08 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMANDS_H
#define COMMANDS_H

typedef struct daemon_server daemon_server_t;					// forward declaration
typedef struct client client_t;		// forward declaration

typedef struct
{
	const char *command;
	const char *usage;
	const char *description;
	int (*fn)(client_t *client, daemon_server_t *server, const char *args);
}	command_t;

# define CMD_OK 0
# define CMD_LOGOUT 1
# define CMD_SPAWN_SHELL 2

int	shield_cmd_help(client_t *client, daemon_server_t *server, const char *args);
int	shield_cmd_shell(client_t *client, daemon_server_t *server, const char *args);
int	shield_cmd_screenshot(client_t *client, daemon_server_t *server, const char *args);
int	shield_cmd_get(client_t *client, daemon_server_t *server, const char *args);
int	shield_cmd_put(client_t *client, daemon_server_t *server, const char *args);
int	shield_cmd_quit(client_t *client, daemon_server_t *server, const char *args);

#endif // COMMANDS_H
