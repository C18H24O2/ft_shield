#include "DaemonServer.hpp"
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <signal.h>

DaemonServer::DaemonServer(char password_hash[32])
{
	memcpy(this->password_hash, password_hash, 32);
	if (FT_SHIELD_MAX_CLIENTS >= 1)
		this->client_id = 0;
	else
		this->client_id = -1;
	bzero(this->pollfd_array, sizeof(pollfd) * (FT_SHIELD_MAX_CLIENTS + 1));
	this->current_conn = 0;

	struct sigaction sa;
	bzero(&sa, sizeof(sa));
	sa.sa_handler = SIG_IGN;

	for (size_t sig = 1; sig < NSIG; ++sig)
	{
		if (sig == SIGKILL || sig == SIGSTOP || sig == SIGSEGV || sig == SIGFPE || sig == SIGILL || sig == SIGBUS || sig == SIGTRAP)
			continue ; // I just might not want to ignore these, just a thought though
		sigaction(sig, &sa, NULL);
	}
}

DaemonServer::~DaemonServer() { }

int DaemonServer::init()
{
	addrinfo hints, *servinfo, *tmp;

	memset(&hints, 0, sizeof(addrinfo));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	protoent *proto_struct = getprotobyname("TCP");
	if (proto_struct == NULL)
		return 1;
	hints.ai_protocol = proto_struct->p_proto;
	hints.ai_flags = AI_PASSIVE;

	int status = getaddrinfo(NULL, FT_SHIELD_PORT_STRING, &hints, &servinfo);
	if (status != 0)
		return 1;
	int opt = 1;
	for (tmp = servinfo; tmp != NULL; tmp = tmp->ai_next)
	{
		this->pollfd_array[0].fd = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol);
		if (this->pollfd_array[0].fd== -1)
			continue;
		if (setsockopt(this->pollfd_array[0].fd, SOL_SOCKET, SO_REUSEADDR,&opt, sizeof(opt)) == -1)
		{
			close(this->pollfd_array[0].fd);
			continue;
		}
		if (bind(this->pollfd_array[0].fd, tmp->ai_addr, tmp->ai_addrlen) == -1)
		{
			close(this->pollfd_array[0].fd);
			continue;
		}
		break;
	}

	if (tmp == NULL)
	{
		freeaddrinfo(servinfo);
		return 1;
	}
	freeaddrinfo(servinfo);
	if (fcntl(this->pollfd_array[0].fd, F_SETFL, O_NONBLOCK) == -1)
	{
		close(this->pollfd_array[0].fd);
		return 1;
	}
	if (listen(this->pollfd_array[0].fd, SOMAXCONN) == -1)
	{
		close(this->pollfd_array[0].fd);
		return 1;
	}

	return 0;
}

void DaemonServer::run()
{
	while (1)
	{
		if (poll(this->pollfd_array, current_conn + 1, FT_SHIELD_TIMEOUT * 1000) == -1)
		{
			// Something happened while poll was waiting, most likely a signal, if we haven't already crashed by now, then its probably fine to continue tbh
			continue ;
		}
		for (size_t i = 0; i < FT_SHIELD_MAX_CLIENTS + 1; i++)		// data receive pass && timeout set pass
		{
			if (this->pollfd_array[i].revents & POLLIN)
			{
				if (i == 0)
				{
					printf("PLACEHOLDER POLL");		// accept a new client
				}
				else
				{
					printf("PLACEHOLDER POLL");		// received message from client
				}
			}
			if (i != 0)
			{
				printf("PLACEHOLDER POLL");			//  for each fd, we check what time it was since last update, and set any inactive client to be disconnected
			}
		}

		for (size_t i = 0; i < FT_SHIELD_MAX_CLIENTS + 1; i++)		// data send pass and disconnect pass
		{
			if (this->pollfd_array[i].revents & POLLOUT)			// send data if needed
			{
				printf("PLACEHOLDER POLL");
				continue;
			}
			if (i != 0 && this->client_list[i -1].state == ClientState::DISCONNECTED) // check for disconnected state, and disconnect user
			{
				printf("PLACEHOLDER POLL");
				continue;
			}
		}
	}
}

