#pragma once

#include <ctime>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <poll.h>

#define FT_SHIELD_MAX_CLIENTS 3
#define FT_SHIELD_PORT 4242
#define FT_SHIELD_PORT_STRING "4242"
#define FT_SHIELD_SHELL "/bin/sh" 
#define FT_SHIELD_TIMEOUT 60		// time in Seconds before a client gets dropped (from idling in auth)

enum class ClientState
{
	CONNECTED,		// Initial state, awaiting authentication
	AUTHENTICATED,  // Authenticated, awaiting command
	SHELL,			// Special state where the client is directly connected to the shell
	DISCONNECTED,	// Client has been disconnected (internal management state)
};

typedef struct
{
	struct pollfd pollfd;
	ClientState	state;
	time_t last_seen;
}	Client;

class DaemonServer
{
	private:
		Client client_list[FT_SHIELD_MAX_CLIENTS + 1];	//1 added for the server itself
		int client_id;											//client_list[client_id] is where the next client should be put in the list, -1 is no clients being accepted
		char password_hash[32];

	public:
		DaemonServer(char password_hash[32]);
		~DaemonServer();
		int init();
		void run();
};
