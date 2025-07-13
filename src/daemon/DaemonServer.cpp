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
		this->should_accept = true;
	else
		this->should_accept = false;

	// Initialize both arrays to unused states
	bzero(this->pollfd_array, sizeof(pollfd) * (FT_SHIELD_MAX_CLIENTS + 1));
	for (size_t i = 0; i < FT_SHIELD_MAX_CLIENTS + 1; i++)
	{
		this->pollfd_array[i].fd = -1;		// -1 means unused as poll() man specifies it ignores pollfd if fd is -1
		this->pollfd_array[i].events = 0;
		this->pollfd_array[i].revents = 0;

	}
	bzero(this->client_list, sizeof(Client) * FT_SHIELD_MAX_CLIENTS);
	for (size_t i = 0; i < FT_SHIELD_MAX_CLIENTS; i++)
	{
		this->client_list[i].pollfd = &this->pollfd_array[i + 1];
		this->client_list[i].state = ClientState::UNUSED;
		this->client_list[i].last_seen = 0;
	}
	this->current_conn = 0;

	struct sigaction sa;
	bzero(&sa, sizeof(sa));
	sa.sa_handler = SIG_IGN;

	// yes this is terrible
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

	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_STREAM;
	protoent *proto_struct = getprotobyname("TCP");
	if (proto_struct == NULL)
		return 1;
	hints.ai_protocol = proto_struct->p_proto;
	hints.ai_flags = AI_PASSIVE;

	int status = getaddrinfo(NULL, FT_SHIELD_PORT_STRING, &hints, &servinfo);
	if (status != 0)
		return 1;
	const int opt_on = 1;
	const int opt_off = 0;
	for (tmp = servinfo; tmp != NULL; tmp = tmp->ai_next)
	{
		this->pollfd_array[0].fd = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol);
		if (this->pollfd_array[0].fd== -1)
			continue;
		if (setsockopt(this->pollfd_array[0].fd, SOL_SOCKET, SO_REUSEADDR, &opt_on, sizeof(opt_on)) == -1)
		{
			close(this->pollfd_array[0].fd);
			continue;
		}
		if (setsockopt(this->pollfd_array[0].fd, IPPROTO_IPV6, IPV6_V6ONLY, &opt_off, sizeof(opt_off)) == -1)
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

void DaemonServer::accept_new_client()
{
	// discarding sockaddr for now, can be added later
	int client_fd = accept(this->pollfd_array[0].fd, NULL, NULL);
	
	if (client_fd == -1)
	{
		// no specific behavior for now
		return ;
	}

	if (this->current_conn >= FT_SHIELD_MAX_CLIENTS || this->should_accept == false)	// may add a message to the client later
	{
		close(client_fd);
		return ;
	}

	if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1)	// try to set client socket as non blocking
	{
		close(client_fd);
		return ;
	}

	for (size_t i = 0; i < FT_SHIELD_MAX_CLIENTS; i++)
	{
		if (this->client_list[i].state == ClientState::UNUSED)
		{
			this->pollfd_array[i + 1].fd = client_fd;
			this->client_list[i].state = ClientState::CONNECTED;
			time(&this->client_list[i].last_seen);
			this->pollfd_array[i + 1].events = POLLIN | POLLOUT;	// we want to receive data and send data
			this->current_conn++;
			return ;
		}
	}
}

void DaemonServer::clear_client(Client *client)
{
	if (client == NULL)
		return ;

	client->pollfd->fd = -1;
	client->pollfd->events = 0;

	client->state = ClientState::UNUSED;
	client->last_seen = 0;
	client->input_buffer.clear();
	client->output_buffer.clear();
}

void DaemonServer::clear_client(size_t client_index)
{
	if (client_index >= FT_SHIELD_MAX_CLIENTS)
		return ;
	clear_client(&this->client_list[client_index]);
}

void DaemonServer::disconnect_client(Client *client)
{
	if (client == NULL)
		return ;

	close(client->pollfd->fd);
	clear_client(client);
	this->current_conn--;
}

void DaemonServer::disconnect_client(size_t client_index)
{
	if (client_index >= FT_SHIELD_MAX_CLIENTS)
		return ;
	disconnect_client(&this->client_list[client_index]);
}

void DaemonServer::receive_message(Client *client)
{
	if (client == NULL || client->state == ClientState::UNUSED)
		return ;

	char buffer[FT_SHIELD_MESSAGE_SIZE];	// uuuh luh 4kB size oui oui (way too much for what we need but eh)

	while (1)	//loop on that thang
	{
		ssize_t rec_bytes = recv(client->pollfd->fd, buffer, FT_SHIELD_MESSAGE_SIZE, 0);
		if ( rec_bytes <= 0)
		{
			if (rec_bytes == 0) // client disconnected
				this->disconnect_client(client);
			else if (errno != EAGAIN && errno != EWOULDBLOCK) // some other error occurred
				this->disconnect_client(client);
			return ;
		}
		client->input_buffer.append(buffer, rec_bytes);
		if (rec_bytes < sizeof(buffer))
			break ;
	}
}

void DaemonServer::receive_message(size_t client_index)
{
	if (client_index >= FT_SHIELD_MAX_CLIENTS)
		return ;
	receive_message(&this->client_list[client_index]);
}

void DaemonServer::send_message(Client *client)
{
	if (client == NULL || client->state == ClientState::UNUSED || client->output_buffer.empty())
		return ;

	ssize_t sent_bytes = send(client->pollfd->fd, client->output_buffer.c_str(), client->output_buffer.size(), 0);
	if (sent_bytes <= 0)
	{
		if (sent_bytes == 0) // client disconnected
			this->disconnect_client(client);
		else if (errno != EAGAIN && errno != EWOULDBLOCK) // some other error occurred
			this->disconnect_client(client);
		return ;
	}
	client->output_buffer.erase(0, sent_bytes); // remove the bytes that were sent
}

void DaemonServer::send_message(size_t client_index)
{
	if (client_index >= FT_SHIELD_MAX_CLIENTS)
		return ;
	send_message(&this->client_list[client_index]);
}

void DaemonServer::check_activity(Client *client)
{
	if (client == NULL)
		return ;

	time_t current_time;
	time(&current_time);
	if (current_time - client->last_seen >= FT_SHIELD_TIMEOUT)
	{
		client->state = ClientState::DISCONNECTED;	// set client to be disconnected
	}
}

void DaemonServer::check_activity(size_t client_index)
{
	if (client_index >= FT_SHIELD_MAX_CLIENTS)
		return ;
	check_activity(&this->client_list[client_index]);
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
					this->accept_new_client();		// received message on the server socket, accept new client
				}
				else
				{
					this->receive_message(i - 1);	// received message on a client socket, receive message
				}
			}
			if (i != 0 && this->client_list[i - 1].state == ClientState::CONNECTED)
			{
				this->check_activity(i - 1);		//  for each fd, we check what time it was since last update, and set any inactive client to be disconnected
			}
		}

		for (size_t i = 0; i < FT_SHIELD_MAX_CLIENTS; i++)		// data send pass and disconnect pass
		{
			if (this->pollfd_array[i + 1].revents & POLLOUT && this->client_list[i].output_buffer.size() != 0)		// data send
			{
				this->send_message(i);	// send message to client
				continue;
			}
			if (i != 0 && this->client_list[i].state == ClientState::DISCONNECTED)		// check for disconnected state, and disconnect user
			{
				this->disconnect_client(i);
				continue;
			}
		}
	}
}

