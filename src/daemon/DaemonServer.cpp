#include "DaemonServer.hpp"
#include <shield/le_function.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <signal.h>
#include "shield.h"

int sig_received = 0;

void handle_signals(int sig)
{
	sig_received = sig;
	DEBUG("Received signal!\n");
	return ;
}

DaemonServer::DaemonServer(char password_hash[32])
{
	memcpy(this->password_hash, password_hash, 32);
	if (FT_SHIELD_MAX_CLIENTS >= 1)
		this->should_accept = true;
	else
		this->should_accept = false;

	// Initialize both arrays to unused states
	for (size_t i = 0; i < FT_SHIELD_MAX_CLIENTS + 1; i++)
	{
		this->pollfd_array[i].fd = -1;		// -1 means unused as poll() man specifies it ignores pollfd if fd is -1
		this->pollfd_array[i].events = 0;
		this->pollfd_array[i].revents = 0;
	}
	for (size_t i = 0; i < FT_SHIELD_MAX_CLIENTS; i++)
	{
		this->client_list[i].index = i;
		this->client_list[i].pollfd = &this->pollfd_array[i + 1];
		this->client_list[i].state = ClientState::UNUSED;
		this->client_list[i].last_seen = 0;
		this->client_list[i].input_buffer.clear();
		this->client_list[i].output_buffer.clear();
	}
	this->current_conn = 0;
}

DaemonServer::~DaemonServer() { }

int DaemonServer::init()
{
#if MATT_MODE
	if (this->logger.init(MATT_LOGFILE_DIR, MATT_LOGFILE) != 0)
		return 1;
	MLOG("Server initializing...");
#endif

	addrinfo hints, *servinfo, *tmp;

	memset(&hints, 0, sizeof(addrinfo));

	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_STREAM;
	protoent *proto_struct = getprotobyname("TCP");
	if (proto_struct == NULL)
	{
		MERR("getprotobyname failed\n");
		return 1;
	}
	hints.ai_protocol = proto_struct->p_proto;
	hints.ai_flags = AI_PASSIVE;

	int status = getaddrinfo(NULL, FT_SHIELD_PORT_STRING, &hints, &servinfo);
	if (status != 0)
	{
		MERR("getaddrinfo failed\n");
		return 1;
	}
	const int opt_on = 1;
	const int opt_off = 0;
	for (tmp = servinfo; tmp != NULL; tmp = tmp->ai_next)
	{
		this->pollfd_array[0].fd = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol);
		if (this->pollfd_array[0].fd== -1)
			continue;
		DEBUG("Created socket %d\n", this->pollfd_array[0].fd);
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
		MLOG("Bound successfully to " + std::string(inet_ntoa(((struct sockaddr_in*)tmp->ai_addr)->sin_addr)) + ":" + std::to_string(ntohs(((struct sockaddr_in*)tmp->ai_addr)->sin_port)) + "\n");
		break;
	}

	if (tmp == NULL)
	{
		MERR("Failed to bind to any address\n");
		freeaddrinfo(servinfo);
		return 1;
	}
	freeaddrinfo(servinfo);
	if (fcntl(this->pollfd_array[0].fd, F_SETFL, O_NONBLOCK) == -1)
	{
		MERR("Failed to set server socket as non-blocking\n");
		close(this->pollfd_array[0].fd);
		return 1;
	}
	if (listen(this->pollfd_array[0].fd, SOMAXCONN) == -1)
	{
		MERR("Failed to listen on server socket\n");
		close(this->pollfd_array[0].fd);
		return 1;
	}

	this->pollfd_array[0].events = POLLIN;	// we want to receive data on the server socket
	this->pollfd_array[0].revents = 0;

	// Handle all possible (reasonable) signals
	for (int i = 1; i < NSIG; i++) {
		if (i == SIGCONT || i == SIGWINCH || i == SIGTSTP || i == SIGTTIN || i == SIGTTOU)
			continue;
		signal(i, handle_signals); 
	}

	MLOG("Server initialized");

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
			this->client_list[i].index = this->current_conn;
			this->pollfd_array[i + 1].fd = client_fd;
			this->client_list[i].state = ClientState::CONNECTED;
			time(&this->client_list[i].last_seen);
			this->pollfd_array[i + 1].events = POLLIN;	// we want to receive data and send data
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
	client->pollfd->revents = 0;

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

	client->input_buffer.clear();
	while (1)	//loop on that thang
	{
		ssize_t rec_bytes = recv(client->pollfd->fd, buffer, FT_SHIELD_MESSAGE_SIZE, 0);
		if (rec_bytes <= 0)
		{
			DEBUG("client disconnected or error occurred\n");
			if (rec_bytes == 0) // client disconnected
				client->state = ClientState::DISCONNECTED;
			else if (errno != EAGAIN && errno != EWOULDBLOCK) // some other error occurred
				client->state = ClientState::DISCONNECTED;
			return ;
		}
		client->input_buffer.append(buffer, rec_bytes);
		if (rec_bytes < FT_SHIELD_MESSAGE_SIZE)
			break ;
	}
	MLOG("Received message from client " + std::to_string(client->index) + ": " + client->input_buffer);
}

#if SHIELD_DEBUG
void print_client_info(const Client &client)
{
	DEBUG("CLIENT INFO:\n");
	DEBUG("  Index: %d\n", (int) client.index);
	DEBUG("  State: %d\n", static_cast<int>(client.state));
	DEBUG("  Last seen: %ld\n", client.last_seen);
	DEBUG("  Input buffer size: %d\n", (int) client.input_buffer.size());
	DEBUG("  Output buffer size: %d\n", (int) client.output_buffer.size());
	DEBUG("  Pollfd fd: %d\n", client.pollfd->fd);
	DEBUG("  Pollfd events: %d\n", client.pollfd->events);
	DEBUG("  Pollfd revents: %d\n", client.pollfd->revents);
	DEBUG("  Input buffer content: %s\n", client.input_buffer.c_str());
	DEBUG("  Output buffer content: %s\n", client.output_buffer.c_str());
	DEBUG("END OF CLIENT INFO\n");
}
#else
# define print_client_info(x)
#endif

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
		DEBUG("client disconnected or error occurred\n");
		if (sent_bytes == 0) // client disconnected
			client->state = ClientState::DISCONNECTED; // set client to be disconnected
		else if (errno != EAGAIN && errno != EWOULDBLOCK) // some other error occurred
			client->state = ClientState::DISCONNECTED; // set client to be disconnected
		return ;
	}
	client->output_buffer.erase(0, sent_bytes); // remove the bytes that were sent
	client->pollfd->events = POLLIN; // reset events to only POLLIN, as we don't want to send data anymore
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
	MLOG("Server running, process id: " + std::to_string(le_getpid()) + "\n");

	while (!sig_received)
	{
		DEBUG("Polling for events on %d connections\n", current_conn);
		if (poll(this->pollfd_array, FT_SHIELD_MAX_CLIENTS + 1, FT_SHIELD_TIMEOUT * 1000) == -1)
		{
			if (sig_received != 0)
			{
				#if MATT_MODE
				this->logger.info("Received signal (" + std::string(le_strsignal(sig_received)) + "), stopping server");
				#endif
				break ;
			}
			MLOG("Poll failed, continuing");
			continue ;
		}
		DEBUG("got %d events\n", this->pollfd_array[0].revents);
		for (size_t i = 0; i < FT_SHIELD_MAX_CLIENTS + 1; i++)		// data receive pass && timeout set pass
		{
			// DEBUG("receive/connect %zu\n", i);

			if (this->pollfd_array[i].revents & POLLIN)
			{
				DEBUG("Received event on fd %zu\n", i);
				if (i == 0)
				{
					DEBUG("New client trying to connect\n");
					this->pollfd_array[i].revents = 0;	// reset revents for the server socket
					this->accept_new_client();		// received message on the server socket, accept new client
					DEBUG("Accepted new client, current connections: %d\n", this->current_conn);
				}
				else
				{
					// print_client_info(this->client_list[i - 1]); // Print client info for debugging
					DEBUG("Received message on client socket %zu\n", i - 1);
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
			// DEBUG("send/disconnect %zu\n", i);
			print_client_info(this->client_list[i]); // Print client info for debugging
			if (this->pollfd_array[i + 1].revents & POLLOUT && this->client_list[i].output_buffer.size() != 0)		// data send
			{
				DEBUG("Sending message to client %zu\n", i);
				this->send_message(i);	// send message to client
				continue;
			}
			if (this->client_list[i].state == ClientState::DISCONNECTED)		// check for disconnected state, and disconnect user
			{
				DEBUG("Client %zu disconnected\n", i);
				this->disconnect_client(i);
				continue;
			}
		}
	}

	MLOG("Server stopped");
}

