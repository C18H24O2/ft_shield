#include <shield/guard.h>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif // !_GNU_SOURCE

CPPGUARD_START
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
CPPGUARD_END

#include <shield/qio.h>
#include <shield/server.h>

static int sig_received = 0;

void handle_signals(int sig)
{
	DEBUG("Received signal '%s' (%d)!\n", strsignal(sig), sig);
	if (sig == SIGUSR1)
		sig_received = sig;
#if SHIELD_DEBUG
	if (sig == SIGINT)
	{
		sig_received = sig;
		DEBUG("SHIELD_DEBUG is enabled, exiting.\n");
	}
#endif
	return ;
}

void server_cleanup(daemon_server_t *that)
{
	for (size_t i = 0; i < FT_SHIELD_MAX_CLIENTS; i++) {
		if (that->client_list[i].state != CLIENT_UNUSED)
			server_disconnect_client(that, i);
		kr_strdel(&that->client_list[i].in_buffer);
		kr_strdel(&that->client_list[i].out_buffer);
	}
	close(that->pollfd_array[0].fd);
}

int server_init(daemon_server_t *that)
{
	that->should_accept = false;
	if (FT_SHIELD_MAX_CLIENTS >= 1)
		that->should_accept = true;

	// Initialize both arrays to unused states
	for (size_t i = 0; i < MAX_FD; i++)
	{
		that->pollfd_array[i].fd = -1;					// -1 means unused as poll() man specifies it ignores pollfd if fd is -1
		that->pollfd_array[i].events = 0;
		that->pollfd_array[i].revents = 0;

		that->poll_metadata[i].client_index = -1;		//All metadata is setup for poll, -1 indicates unused fd, or the server fd
		that->poll_metadata[i].fd_type = FD_UNUSED;
	}
	for (size_t i = 0; i < FT_SHIELD_MAX_CLIENTS; i++)
	{
		that->client_list[i].index = i;
		that->client_list[i].pollfd = NULL;
		that->client_list[i].pty_fd = -1;
		that->client_list[i].state = CLIENT_UNUSED;
		that->client_list[i].last_seen = 0;
		that->client_list[i].in_buffer = kr_string_empty;
		that->client_list[i].out_buffer = kr_string_empty;
		that->client_list[i].metadata = NULL;
	}
	that->shell_next = false;
	that->current_conn = 0;

#if MATT_MODE
	if (that->logger.init(MATT_LOGFILE_DIR, MATT_LOGFILE) != 0)
		return 1;
	MLOG("Server initializing...\n");
#endif

	struct addrinfo hints, *servinfo, *tmp;
	memset(&hints, 0, sizeof(struct addrinfo));

	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_STREAM;
	struct protoent *proto_struct = getprotobyname("TCP");
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
		that->pollfd_array[0].fd = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol);
		if (that->pollfd_array[0].fd == -1)
			continue;
		DEBUG("Created socket %d\n", that->pollfd_array[0].fd);
		if (setsockopt(that->pollfd_array[0].fd, SOL_SOCKET, SO_REUSEADDR, &opt_on, sizeof(opt_on)) == -1)
		{
			close(that->pollfd_array[0].fd);
			continue;
		}
		if (setsockopt(that->pollfd_array[0].fd, IPPROTO_IPV6, IPV6_V6ONLY, &opt_off, sizeof(opt_off)) == -1)
		{
			close(that->pollfd_array[0].fd);
			continue;
		}
		if (bind(that->pollfd_array[0].fd, tmp->ai_addr, tmp->ai_addrlen) == -1)
		{
			close(that->pollfd_array[0].fd);
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
	if (fcntl(that->pollfd_array[0].fd, F_SETFL, O_NONBLOCK) == -1)
	{
		MERR("Failed to set server socket as non-blocking.");
		close(that->pollfd_array[0].fd);
		return 1;
	}
	if (listen(that->pollfd_array[0].fd, SOMAXCONN) == -1)
	{
		MERR("Failed to listen on server socket.");
		close(that->pollfd_array[0].fd);
		return 1;
	}

	that->pollfd_array[0].events = POLLIN;	// we want to receive data on the server socket
	that->pollfd_array[0].revents = 0;

	that->poll_metadata[0].fd_type = FD_SERVER;

	// Handle all possible (reasonable) signals
	struct sigaction sa;
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = handle_signals;
	sa.sa_flags = 0;
	for (int i = 1; i < NSIG; i++) {
		// SIGRT_32 because valgrind whines about it. also yes, SIGRTMIN is SIGRT_2, don't ask me why
		if (i == SIGRTMIN + 30)
			continue;
		// a bunch of signals that are either uncatchable or dumb to catch
		if (i == SIGKILL || i == SIGSTOP || i == SIGTSTP)
			continue;
		// No. We will NOT catch a Segfault. I *want* to crash. I want a stacktrace. I want actual useful errors. Bite me.
		if (i == SIGSEGV)
			continue;
		if (sigaction(i, &sa, NULL) == -1) {
			// if this fails that means either we are in a sandbox and we can't trust nobody, or the kernel will explode by..... about now, give or take.
			MERR("sigaction failed for signal " + std::to_string(i));
		}
	}

	MLOG("Server initialized.");
	return 0;
}

void server_accept_new_client(daemon_server_t *that)
{
	// discarding sockaddr for now, can be added later
	int client_fd = accept(that->pollfd_array[0].fd, NULL, NULL);
	
	if (client_fd == -1)
	{
		// no specific behavior for now
		return ;
	}

	if (that->current_conn >= FT_SHIELD_MAX_CLIENTS || that->should_accept == false)	// may add a message to the client later
	{
		shutdown(client_fd, SHUT_RDWR);
		close(client_fd);
		return ;
	}

	if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1)	// try to set client socket as non blocking
	{
		shutdown(client_fd, SHUT_RDWR);
		close(client_fd);
		return ;
	}

	qio_data.total_connections++;

	for (size_t i = 0; i < FT_SHIELD_MAX_CLIENTS; i++)
	{
		if (that->client_list[i].state == CLIENT_UNUSED)
		{
			for (size_t j = 0; j < MAX_FD; j++)
			{
				if (that->poll_metadata[j].fd_type == FD_UNUSED)
				{
					that->pollfd_array[j].fd = client_fd;
					that->pollfd_array[j].events |= POLLIN;	// we always want to poll for incoming data from a client, POLLOUT will be set when we have data to send
					that->pollfd_array[j].revents = 0;		// reset revents
					that->poll_metadata[j].client_index = i;
					that->poll_metadata[j].fd_type = FD_CLIENT_SOCKET;
					that->client_list[i].metadata = &that->poll_metadata[j];
					that->client_list[i].pollfd = &that->pollfd_array[j];
					break;
				}
			}
			that->client_list[i].index = i;
			that->client_list[i].state = CLIENT_CONNECTED;
			time(&that->client_list[i].last_seen);
			that->current_conn++;
			return ;
		}
	}
	shutdown(client_fd, SHUT_RDWR);
	close(client_fd); // fuck you
}

void server_clear_client(daemon_server_t *that, client_t *client)
{
	(void)that;
	if (client == NULL)
		return ;

	if (client->pollfd != NULL)
	{
		client->pollfd->fd = -1;
		client->pollfd->events = 0;
		client->pollfd->revents = 0;
	}
	if (client->metadata != NULL)
	{
		client->metadata->fd_type = FD_UNUSED;
		client->metadata->client_index = -1;
	}

	client->pollfd = NULL;
	client->metadata = NULL;

	client->state = CLIENT_UNUSED;
	client->pty_fd = -1;
	client->last_seen = 0;
	
	// clear but don't free them yet, we wouldn't want to have to re-allocate memory down the line now do we
	kr_strclr(&client->in_buffer);
	kr_strclr(&client->out_buffer);
}

void server_disconnect_client(daemon_server_t *that, size_t client_index)
{
	if (client_index >= FT_SHIELD_MAX_CLIENTS)
		return ;
	client_t *client = &(that->client_list[client_index]);
	if (client == NULL)
		return ;

	shutdown(client->pollfd->fd, SHUT_RDWR);
	close(client->pollfd->fd);

	server_clear_client(that, client);
	that->current_conn--;
}

bool server_receive_message(daemon_server_t *that, size_t client_index)
{
	if (client_index >= FT_SHIELD_MAX_CLIENTS)
		return (true);
	client_t *client = &(that->client_list[client_index]);
	if (client == NULL || client->state == CLIENT_UNUSED)
		return (true);

	char buffer[FT_SHIELD_MESSAGE_SIZE + 1];	// uuuh luh 4kB size oui oui (way too much for what we need but eh)

	DEBUG("Receiving message from client %d\n", client->index);
	while (1)	//loop on that thang
	{
		memset(buffer, 0, FT_SHIELD_MESSAGE_SIZE + 1);
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
		qio_data.bytes_received += rec_bytes;
		DEBUG("Received %zu bytes\n", rec_bytes);
		if (!kr_strappend(&client->in_buffer, buffer))
			break;
		//TODO: why is this here? shouldn't we keep accepting until maybe we get a \n?
		// and if we don't want to, if it's not already there, let's maybe send the \n
		// ourselves so the client is not "misaligned"
		if (rec_bytes < FT_SHIELD_MESSAGE_SIZE)
			break;
	}
#if MATT_MODE
	if (kr_strcmp(&client->in_buffer, "quit\n") == 0)
		return (false);
#endif
	DEBUG("User input: %.*s\n", client->in_buffer.len, client->in_buffer.ptr);

	// Here depending on what "mode" the client is in":
	// Standard mode just interprets and looks for commands sent to the server
	// Shell mode passes the data to the connected shell given it is properly cut
	// TODO: cmd + shell send
	
	if (client->pty_fd == -1)
	{
		DEBUG("input to interpret: %.*s\n", (int) client->in_buffer.len, client->in_buffer.ptr);

		size_t len = kr_strcspn(&client->in_buffer, " \n");
		kr_strview_t command = kr_strsubst(&client->in_buffer, 0, len);

		DEBUG("command: %.*s\n", (int) command.len, command.ptr);

		for (int i = 0; commands[i].command; i++)
		{
			if (kr_strcmp(&command, commands[i].command) == 0)
			{
				DEBUG("Command %s found\n", commands[i].command);
				commands[i].fn(client, that, &client->in_buffer);
				break;
			}
		}

		kr_strclr(&client->in_buffer);
		return true;
	}

	switch (client->pty_fd)
	{
		case -1:	//The client does not Have a pty linked -> Normal mode
			break;

		default:	// anything else *should* be that the client has a shell
			DEBUG("Passing data to shell pty\n");
			size_t written = 0;
			while (written < client->in_buffer.len)
			{
				int wbytes = write(client->pty_fd, client->in_buffer.ptr + written, client->in_buffer.len - written);
				if (wbytes <= 0)
				{
					DEBUG("Error writing to pty\n");
					client->state = CLIENT_DISCONNECTED;
					return (true);
				}
				written += wbytes;
			}
			break;
	}
	kr_strclr(&client->in_buffer);

	return (true);
}

#if SHIELD_DEBUG
void print_client_info(client_t *client)
{
	DEBUG("CLIENT INFO:\n");
	DEBUG("  Index: %d\n", (int) client->index);
	DEBUG("  State: %d\n", (int) (client->state));
	DEBUG("  Last seen: %ld\n", client->last_seen);
	DEBUG("  Input buffer size: %d\n", (int) client->in_buffer.len);
	DEBUG("  Output buffer size: %d\n", (int) client->out_buffer.len);
	DEBUG("  Pollfd fd: %d\n", client->pollfd->fd);
	DEBUG("  Pollfd events: %d\n", client->pollfd->events);
	DEBUG("  Pollfd revents: %d\n", client->pollfd->revents);
	DEBUG("  Input buffer content: %.*s\n", client->in_buffer.ptr);
	DEBUG("  Output buffer content: %.*s\n", client->out_buffer.ptr);
	DEBUG("END OF CLIENT INFO\n");
}
#else
# define print_client_info(x)
#endif

void server_send_message(daemon_server_t *that, size_t client_index)
{
	if (client_index >= FT_SHIELD_MAX_CLIENTS)
		return ;
	client_t *client = &(that->client_list[client_index]);
	if (client == NULL || client->state == CLIENT_UNUSED || client->out_buffer.len == 0)
		return ;

	size_t written = 0;
	while (written < client->out_buffer.len)
	{
		ssize_t sent_bytes = send(client->pollfd->fd, client->out_buffer.ptr + written, client->out_buffer.len - written, 0);
		if (sent_bytes <= 0)
		{
			DEBUG("client disconnected or error occurred\n");
			if (sent_bytes == 0) // client disconnected
				client->state = CLIENT_DISCONNECTED; // set client to be disconnected
			else if (errno != EAGAIN && errno != EWOULDBLOCK) // some other error occurred
				client->state = CLIENT_DISCONNECTED; // set client to be disconnected
			return ;
		}
		qio_data.bytes_sent += sent_bytes;
		written += sent_bytes;
	}
	kr_strclr(&client->out_buffer);
	client->pollfd->events &= ~POLLOUT; // remove pollout from events to check
}

void server_check_activity(daemon_server_t *that, size_t client_index)
{
	if (client_index >= FT_SHIELD_MAX_CLIENTS)
		return ;
	client_t *client = &(that->client_list[client_index]);
	if (client == NULL)
		return ;

	time_t current_time;
	time(&current_time);
	if (current_time - client->last_seen >= FT_SHIELD_TIMEOUT)
	{
		client->state = CLIENT_DISCONNECTED;	// set client to be disconnected
	}
}

void server_receive_shell_data(daemon_server_t *that, size_t client_index)
{
	if (client_index >= FT_SHIELD_MAX_CLIENTS)
		return ;
	client_t *client = &(that->client_list[client_index]);
	if (client->pty_fd == -1)
		return ;

	char buffer[FT_SHIELD_MESSAGE_SIZE + 1];
	memset(buffer, 0, FT_SHIELD_MESSAGE_SIZE + 1);
	ssize_t rec_bytes = read(client->pty_fd, buffer, FT_SHIELD_MESSAGE_SIZE);
	if (rec_bytes <= 0)
	{
		DEBUG("Error reading from pty\n");
		client->state = CLIENT_DISCONNECTED;
		return ;
	}
	kr_strappend(&client->out_buffer, buffer);
	client->pollfd->events |= POLLOUT;
}

void server_send_shell_data(daemon_server_t *that, size_t client_index)
{
	(void)client_index;
	(void)that;
	return;
}

void server_run(daemon_server_t *that)
{
	MLOG("Server running, process id: " + std::to_string(getpid()));

	while (!sig_received || sig_received == SIGUSR1)
	{
		if (sig_received == SIGUSR1)
		{
			MLOG("Another daemon instance is already running, not starting another.");
			sig_received = 0;
		}
		DEBUG("Polling for events on %d connections\n", that->current_conn);
		if (poll(that->pollfd_array, MAX_FD, FT_SHIELD_TIMEOUT * 1000) == -1)	//At the club, straight up polling it, and by it lets jsut say.. my static array 
		{
			if (sig_received == SIGUSR1)
			{
				MLOG("Another daemon instance is already running, not starting another.");
				sig_received = 0;
			}
			if (sig_received != 0)
			{
				MLOG("Received signal (" + std::string(strsignal(sig_received)) + "), stopping server.");
				break ;
			}
			DEBUG("Poll failed, continuing.\n");
			continue ;
		}
		DEBUG("got %d events\n", that->pollfd_array[0].revents);
		bool quit = false;
		for (size_t i = 0; i < MAX_FD; i++)		// data receive pass && timeout set pass
		{
			if (that->pollfd_array[i].fd < 0)	// Skip any unused fd
				continue;
			if (that->pollfd_array[i].revents == 0)	// No events on that fd
				continue;
			DEBUG("fd is of type %d\n", that->poll_metadata[i].fd_type);
			switch (that->poll_metadata[i].fd_type)
			{
				case FD_SERVER:		//Accept new clients
				{
					DEBUG("New client trying to connect\n");
					that->pollfd_array[i].revents = 0;	// reset revents for the server socket
					server_accept_new_client(that);		// received message on the server socket, accept new client
					DEBUG("Accepted new client, current connections: %d\n", that->current_conn);
					break;
				}
				case FD_CLIENT_SOCKET:	//check for received data from client socket
				{
					size_t client_index = (size_t)that->poll_metadata[i].client_index;
					DEBUG("Checking client socket %zu\n", client_index);
					if (that->pollfd_array[i].revents & POLLIN)
					{
						DEBUG("Received data from client %zu\n", client_index);
						if (!server_receive_message(that, client_index))
						{
							MLOG("Received quit command (client " + std::to_string(client_index) + "), exiting.");
							quit = true;
						}
					}
					if (that->pollfd_array[i].revents & (POLLHUP | POLLERR)) //if the client hangs up or if the socket has an error we disconnect the client
					{
						DEBUG("Client %zu error or hangup\n", client_index);
						that->client_list[client_index].state = CLIENT_DISCONNECTED;
					}
					break;
				}
				case FD_CLIENT_PTY:	//check for client's pseudo-terminal
				{
					// I don't know what the fuck im doing here
					size_t client_index = (size_t)that->poll_metadata[i].client_index;
					if (that->pollfd_array[i].revents & (POLLHUP | POLLERR))
					{
						// the shell is closed, cleanup
						shutdown(that->pollfd_array[i].fd, SHUT_RDWR);
						close(that->pollfd_array[i].fd);
						that->pollfd_array[i].fd = -1;
						that->pollfd_array[i].events = 0;
						that->pollfd_array[i].revents = 0;

						that->poll_metadata[i].client_index = -1;
						that->poll_metadata[i].fd_type = FD_UNUSED;
						that->client_list[client_index].pty_fd = -1;
						
						break;
					}
					if (that->client_list[client_index].state == CLIENT_DISCONNECTED)
						break;
					if (that->pollfd_array[i].revents & POLLIN) //bro has shit to say
					{
						server_receive_shell_data(that, client_index);
					}
					break;
				}
				default:
					break;
			}
			if (quit == true)
				break;
			
			// if (that->pollfd_array[i].revents & POLLIN)
			// {
			// 	DEBUG("Received event on fd %zu\n", i);
			// 	if (i == 0)
			// 	{
			// 		DEBUG("New client trying to connect\n");
			// 		that->pollfd_array[i].revents = 0;	// reset revents for the server socket
			// 		server_accept_new_client(that);		// received message on the server socket, accept new client
			// 		DEBUG("Accepted new client, current connections: %d\n", that->current_conn);
			// 	}
			// 	else
			// 	{
			// 		// print_client_info(&(that->client_list[i - 1])); // Print client info for debugging
			// 		DEBUG("Received message on client socket %zu\n", i - 1);
			// 		if (!server_receive_message(that, i - 1)) {	// received message on a client socket, receive message
			// 			// We want to quit, explode.
			// 			MLOG("Received quit command (client " + std::to_string(i - 1) + "), exiting.");
			// 			quit = true;
			// 			break;
			// 		}
			// 	}
			// }
			// if (i != 0 && that->client_list[i - 1].state == CLIENT_CONNECTED)
			// {
			// 	that->check_activity(that, i - 1);		//  for each fd, we check what time it was since last update, and set any inactive client to be disconnected
			// }
		}
		if (quit)
			break;

		for (size_t i = 0; i < MAX_FD; i++)		// data send pass and disconnect pass
		{
			if (that->pollfd_array[i].fd < 0)	// Skip any unused fd
				continue;

			switch(that->poll_metadata[i].fd_type)
			{
				case FD_SERVER:
				{
					// nothing to do for server fd here (shut up compiler)
					break;
				}
				case FD_CLIENT_SOCKET:
				{
					if (that->client_list[that->poll_metadata[i].client_index].state == CLIENT_DISCONNECTED)
					{
						DEBUG("Disconnecting client %zu\n", that->poll_metadata[i].client_index);
						server_disconnect_client(that, that->poll_metadata[i].client_index);
						break;
					}
					if (that->pollfd_array[i].revents & POLLOUT && that->client_list[that->poll_metadata[i].client_index].out_buffer.len != 0)		// data send
					{
						DEBUG("Sending message to client %d\n", that->poll_metadata[i].client_index);
						server_send_message(that, that->poll_metadata[i].client_index);	// send message to client
					}
					break;
				}
				case FD_CLIENT_PTY:
				{
					// nothing to do for pty fd here (shut up compiler)
					break;
				}
				default:
					break;
			}
			// DEBUG("send/disconnect %zu\n", i);
			// print_client_info(&(that->client_list[i])); // Print client info for debugging
			// if (that->pollfd_array[i + 1].revents & POLLOUT && that->client_list[i].output_buffer.size() != 0)		// data send
			// {
			// 	DEBUG("Sending message to client %zu\n", i);
			// 	server_send_message(that, i);	// send message to client
			// 	continue;
			// }
			// if (that->client_list[i].state == CLIENT_DISCONNECTED)		// check for disconnected state, and disconnect user
			// {
			// 	DEBUG("Client %zu disconnected\n", i);
			// 	server_disconnect_client(that, i);
			// 	continue;
			// }
		}
	}

	MLOG("Server stopped.");
}

