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
#include <signal.h>
#include "shield.h"

int sig_received = 0;

void handle_signals(int sig)
{
	sig_received = sig;
	DEBUG("Received signal '%s' (%d)!\n", le_strsignal(sig), sig);
	return ;
}

DaemonServer::DaemonServer()
{
	if (FT_SHIELD_MAX_CLIENTS >= 1)
		this->should_accept = true;
	else
		this->should_accept = false;

	// Initialize both arrays to unused states
	for (size_t i = 0; i < MAX_FD; i++)
	{
		this->pollfd_array[i].fd = -1;					// -1 means unused as poll() man specifies it ignores pollfd if fd is -1
		this->pollfd_array[i].events = 0;
		this->pollfd_array[i].revents = 0;

		this->poll_metadata[i].client_index = -1;		//All metadata is setup for poll, -1 indicates unused fd, or the server fd
		this->poll_metadata[i].fd_type = FD_UNUSED;
	}
	for (size_t i = 0; i < FT_SHIELD_MAX_CLIENTS; i++)
	{
		this->client_list[i].index = i;
		this->client_list[i].pollfd = &this->pollfd_array[i + 1];
		this->client_list[i].pty_fd = -1;
		this->client_list[i].state = CLIENT_UNUSED;
		this->client_list[i].last_seen = 0;
		this->client_list[i].input_buffer.clear();
		this->client_list[i].output_buffer.clear();
	}
	this->current_conn = 0;
}

DaemonServer::~DaemonServer() {
	for (size_t i = 0; i < FT_SHIELD_MAX_CLIENTS; i++) {
		if (this->client_list[i].state != CLIENT_UNUSED)
			this->disconnect_client(i);
	}
	close(this->pollfd_array[0].fd);
}

int DaemonServer::init()
{
#if MATT_MODE
	if (this->logger.init(MATT_LOGFILE_DIR, MATT_LOGFILE) != 0)
		return 1;
	MLOG("Server initializing...\n");
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
		if (this->pollfd_array[0].fd == -1)
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
		MERR("Failed to bind to any address.");
		freeaddrinfo(servinfo);
		return 1;
	}
	freeaddrinfo(servinfo);
	if (fcntl(this->pollfd_array[0].fd, F_SETFL, O_NONBLOCK) == -1)
	{
		MERR("Failed to set server socket as non-blocking.");
		close(this->pollfd_array[0].fd);
		return 1;
	}
	if (listen(this->pollfd_array[0].fd, SOMAXCONN) == -1)
	{
		MERR("Failed to listen on server socket.");
		close(this->pollfd_array[0].fd);
		return 1;
	}

	this->pollfd_array[0].events = POLLIN;	// we want to receive data on the server socket
	this->pollfd_array[0].revents = 0;

	this->poll_metadata[0].fd_type = FD_SERVER;

	// Handle all possible (reasonable) signals
	struct sigaction sa;
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = handle_signals;
	sa.sa_flags = 0;
	for (int i = 1; i < NSIG; i++) {
		if (sigaction(i, &sa, NULL) == -1) {
			MERR("sigaction failed for signal " + std::to_string(i));
		}
	}

	MLOG("Server initialized.");

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
		if (this->client_list[i].state == CLIENT_UNUSED)
		{
			for (size_t j = 0; j < MAX_FD; j++)
			{
				if (this->poll_metadata->fd_type == FD_UNUSED)
				{
					this->pollfd_array[j].fd = client_fd;
					this->pollfd_array[j].events |= POLLIN;	// we always want to poll for incoming data from a client, POLLOUT will be set when we have data to send
					this->poll_metadata[j].client_index = i;
					this->poll_metadata[j].fd_type = FD_CLIENT_SOCKET;
					break;
				}
			}
			this->client_list[i].index = this->current_conn;
			this->client_list[i].state = CLIENT_CONNECTED;
			time(&this->client_list[i].last_seen);
			this->current_conn++;
			return ;
		}
	}
	close(client_fd); // fuck you
}

void DaemonServer::clear_client(Client *client)
{
	if (client == NULL)
		return ;

	client->pollfd->fd = -1;
	client->pollfd->events = 0;
	client->pollfd->revents = 0;
	client->metadata->fd_type = FD_UNUSED;
	client->metadata->client_index = -1;

	client->state = CLIENT_UNUSED;
	client->pty_fd = -1;
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

bool DaemonServer::receive_message(Client *client)
{
	if (client == NULL || client->state == CLIENT_UNUSED)
		return (true);

	char buffer[FT_SHIELD_MESSAGE_SIZE];	// uuuh luh 4kB size oui oui (way too much for what we need but eh)

	client->input_buffer.clear();
	while (1)	//loop on that thang
	{
		ssize_t rec_bytes = recv(client->pollfd->fd, buffer, FT_SHIELD_MESSAGE_SIZE, 0);
		if (rec_bytes <= 0)
		{
			DEBUG("client disconnected or error occurred\n");
			if (rec_bytes == 0) // client disconnected
				client->state = CLIENT_DISCONNECTED;
			else if (errno != EAGAIN && errno != EWOULDBLOCK) // some other error occurred
				client->state = CLIENT_DISCONNECTED;
			return (true);
		}
		client->input_buffer.append(buffer, rec_bytes);
		if (rec_bytes < FT_SHIELD_MESSAGE_SIZE)
			break;
	}
#if MATT_MODE
	if (client->input_buffer == "quit" || client->input_buffer == "quit\n")
		return (false);
#endif
	MLOG("User input: " + client->input_buffer);

	// TODO: cmd

	return (true);
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

bool DaemonServer::receive_message(size_t client_index)
{
	if (client_index >= FT_SHIELD_MAX_CLIENTS)
		return (true);
	return (receive_message(&this->client_list[client_index]));
}

void DaemonServer::send_message(Client *client)
{
	if (client == NULL || client->state == CLIENT_UNUSED || client->output_buffer.empty())
		return ;

	ssize_t sent_bytes = send(client->pollfd->fd, client->output_buffer.c_str(), client->output_buffer.size(), 0);
	if (sent_bytes <= 0)
	{
		DEBUG("client disconnected or error occurred\n");
		if (sent_bytes == 0) // client disconnected
			client->state = CLIENT_DISCONNECTED; // set client to be disconnected
		else if (errno != EAGAIN && errno != EWOULDBLOCK) // some other error occurred
			client->state = CLIENT_DISCONNECTED; // set client to be disconnected
		return ;
	}
	client->output_buffer.erase(0, sent_bytes); // remove the bytes that were sent
	client->pollfd->events &= ~POLLOUT; // remove pollout from events to check
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
		client->state = CLIENT_DISCONNECTED;	// set client to be disconnected
	}
}

void DaemonServer::check_activity(size_t client_index)
{
	if (client_index >= FT_SHIELD_MAX_CLIENTS)
		return ;
	check_activity(&this->client_list[client_index]);
}

void DaemonServer::receive_shell_data(size_t client_index)
{
	return;
}

void DaemonServer::send_shell_data(size_t client_index)
{
	return;
}

void DaemonServer::run()
{
	MLOG("Server running, process id: " + std::to_string(le_getpid()));

	while (!sig_received || sig_received == SIGUSR1)
	{
		if (sig_received == SIGUSR1)
		{
			MLOG("Another daemon instance is already running, not starting another.");
			sig_received = 0;
		}
		DEBUG("Polling for events on %d connections\n", current_conn);
		if (poll(this->pollfd_array, MAX_FD, FT_SHIELD_TIMEOUT * 1000) == -1)	//At the club, straight up polling it, and by it lets jsut say.. my static array 
		{
			if (sig_received == SIGUSR1)
			{
				MLOG("Another daemon instance is already running, not starting another.");
				sig_received = 0;
			}
			if (sig_received != 0)
			{
				MLOG("Received signal (" + std::string(le_strsignal(sig_received)) + "), stopping server.");
				break ;
			}
			DEBUG("Poll failed, continuing.\n");
			continue ;
		}
		DEBUG("got %d events\n", this->pollfd_array[0].revents);
		bool quit = false;
		for (size_t i = 0; i < MAX_FD; i++)		// data receive pass && timeout set pass
		{
			if (this->pollfd_array[i].fd < 0)	// Skip any unused fd
				continue;

			switch (this->poll_metadata[i].fd_type)
			{
				case FD_SERVER:		//Accept new clients
				{
					DEBUG("New client trying to connect\n");
					this->pollfd_array[i].revents = 0;	// reset revents for the server socket
					this->accept_new_client();			// received message on the server socket, accept new client
					DEBUG("Accepted new client, current connections: %d\n", this->current_conn);
					break;
				}
				case FD_CLIENT_SOCKET:	//check for received data from client socket
				{
					size_t client_index = (size_t)this->poll_metadata[i].client_index;
					if (this->pollfd_array[client_index].revents & POLLIN)
					{
						if (!receive_message(client_index))
						{
							MLOG("Received quit command (client " + std::to_string(client_index) + "), exiting.");
							quit = true;
						}
					}
					if (this->client_list[client_index].state == CLIENT_CONNECTED)		// we check activity on non authenticated clients, to kick anyone afk as they log in
						this->check_activity(client_index);
					if (this->pollfd_array[client_index].revents & (POLLHUP | POLLERR)) //if the client hangs up or if the socket has an error we disconnect the client
					{
						DEBUG("Client %zu error or hangup\n", client_index);
						this->client_list[client_index].state = CLIENT_DISCONNECTED;
					}
					break;
				}
				case FD_CLIENT_PTY:	//check for client's pseudo-terminal
				{
					// I don't know what the fuck im doing here
					size_t client_index = (size_t)this->poll_metadata[i].client_index;
					if (this->client_list[client_index].state == CLIENT_DISCONNECTED)
						break;
					
					if (this->pollfd_array[client_index].revents & POLLIN) //bro has shit to say
					{

					}
					break;
				}
				default:
					break;
			}
			if (quit == true)
				break;
			
			// if (this->pollfd_array[i].revents & POLLIN)
			// {
			// 	DEBUG("Received event on fd %zu\n", i);
			// 	if (i == 0)
			// 	{
			// 		DEBUG("New client trying to connect\n");
			// 		this->pollfd_array[i].revents = 0;	// reset revents for the server socket
			// 		this->accept_new_client();		// received message on the server socket, accept new client
			// 		DEBUG("Accepted new client, current connections: %d\n", this->current_conn);
			// 	}
			// 	else
			// 	{
			// 		// print_client_info(this->client_list[i - 1]); // Print client info for debugging
			// 		DEBUG("Received message on client socket %zu\n", i - 1);
			// 		if (!this->receive_message(i - 1)) {	// received message on a client socket, receive message
			// 			// We want to quit, explode.
			// 			MLOG("Received quit command (client " + std::to_string(i - 1) + "), exiting.");
			// 			quit = true;
			// 			break;
			// 		}
			// 	}
			// }
			// if (i != 0 && this->client_list[i - 1].state == CLIENT_CONNECTED)
			// {
			// 	this->check_activity(i - 1);		//  for each fd, we check what time it was since last update, and set any inactive client to be disconnected
			// }
		}
		if (quit)
			break;

		for (size_t i = 0; i < MAX_FD; i++)		// data send pass and disconnect pass
		{
			if (this->pollfd_array[i].fd < 0)	// Skip any unused fd
				continue;

			switch(this->poll_metadata[i].fd_type)
			{
				case FD_CLIENT_SOCKET:
				{
					//check for messages to send then disconnect them if needed
					break;
				}
				case FD_CLIENT_PTY:
				{
					//check for info from client to send and disconnect??
					break;
				}
			}
			// DEBUG("send/disconnect %zu\n", i);
			// print_client_info(this->client_list[i]); // Print client info for debugging
			// if (this->pollfd_array[i + 1].revents & POLLOUT && this->client_list[i].output_buffer.size() != 0)		// data send
			// {
			// 	DEBUG("Sending message to client %zu\n", i);
			// 	this->send_message(i);	// send message to client
			// 	continue;
			// }
			// if (this->client_list[i].state == CLIENT_DISCONNECTED)		// check for disconnected state, and disconnect user
			// {
			// 	DEBUG("Client %zu disconnected\n", i);
			// 	this->disconnect_client(i);
			// 	continue;
			// }
		}
	}

	MLOG("Server stopped.");
}

