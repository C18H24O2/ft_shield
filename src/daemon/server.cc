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

#define SHIELD_MAX_PASSWORD_TRIES 3
#define PASSWORD_PROMPT "Password: "
#if MATT_MODE
#define COMMAND_PROMPT ""
#else // !MATT_MODE
#define COMMAND_PROMPT "> "
#endif // !MATT_MODE

void handle_signals(int sig)
{
	DEBUG("Received signal '%s' (%d)!\n", strsignal(sig), sig);
	if (sig == SIGUSR1 || sig == SIGCHLD)
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
		that->client_list[i].pty_pollfd = NULL;
		that->client_list[i].pty_metadata = NULL;
		that->client_list[i].state = CLIENT_UNUSED;
		that->client_list[i].last_seen = 0;
		that->client_list[i].in_buffer = kr_string_empty;
		that->client_list[i].out_buffer = kr_string_empty;
		that->client_list[i].shell_active = false;
#if MATT_MODE
		that->client_list[i].out_buffer = kr_strnew(COMMAND_PROMPT);
#else // !MATT_MODE
		that->client_list[i].out_buffer = kr_strnew(PASSWORD_PROMPT);
#endif // !MATT_MODE
		that->client_list[i].metadata = NULL;
	}
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
#if !MATT_MODE // the matt daemon subject tells us we need to catch everything, sure thing buddy, let's catch everything.
		// SIGRT_32 because valgrind whines about it. also yes, SIGRTMIN is SIGRT_2, don't ask me why
		if (i == SIGRTMIN + 30)
			continue;
		// a bunch of signals that are either uncatchable or dumb to catch
		if (i == SIGKILL || i == SIGSTOP || i == SIGTSTP)
			continue;
		// No. We will NOT catch a Segfault. I *want* to crash. I want a stacktrace. I want actual useful errors. Bite me.
		if (i == SIGSEGV)
			continue;
#endif // !MATT_MODE
		sigaction(i, &sa, NULL);
	}

	MLOG("Server initialized.");
	return 0;
}

void close_fd(int fd)
{
	if (fd == -1)
		return ;
	shutdown(fd, SHUT_RDWR);
	close(fd);
	DEBUG("Closed fd %d\n", fd);
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
		close_fd(client_fd);
		return ;
	}

	if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1)	// try to set client socket as non blocking
	{
		close_fd(client_fd);
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
			that->client_list[i].password_tries = 0;
			that->client_list[i].index = i;
			that->client_list[i].state = CLIENT_UNAUTHENTICATED;
			that->client_list[i].shell_active = false;
			time(&that->client_list[i].last_seen);
			that->current_conn++;
			return ;
		}
	}
	close_fd(client_fd); // fuck you
}

void reset_fd(struct pollfd *pollfd, fd_metadata_t *metadata)
{
	if (pollfd != NULL)
	{
		pollfd->fd = -1;
		pollfd->events = 0;
		pollfd->revents = 0;
	}
	if (metadata != NULL)
	{
		metadata->fd_type = FD_UNUSED;
		metadata->client_index = -1;
	}
}

void server_clear_client(daemon_server_t *that, client_t *client)
{
	(void)that;
	if (client == NULL)
		return;

	if (client->metadata != NULL && client->pollfd != NULL)
		reset_fd(client->pollfd, client->metadata);

	client->pollfd = NULL;
	client->metadata = NULL;

	client->state = CLIENT_UNUSED;

	if (client->pty_pollfd != NULL)
	{
		close_fd(client->pty_pollfd->fd);
		if (client->pty_metadata != NULL)
			reset_fd(client->pty_pollfd, client->pty_metadata);
	}
	client->pty_pollfd = NULL;
	client->pty_metadata = NULL;
	client->last_seen = 0;

	client->shell_active = false;
	client->password_tries = 0;
	
	// clear but don't free them yet, we wouldn't want to have to re-allocate memory down the line now do we
	kr_strclr(&client->in_buffer);
	kr_strclr(&client->out_buffer);
#if MATT_MODE
	kr_strappend(&client->out_buffer, COMMAND_PROMPT);
#else // !MATT_MODE
	kr_strappend(&client->out_buffer, PASSWORD_PROMPT);
#endif // !MATT_MODE
}

void server_disconnect_client(daemon_server_t *that, size_t client_index)
{
	if (client_index >= FT_SHIELD_MAX_CLIENTS)
		return ;
	client_t *client = &(that->client_list[client_index]);
	if (client == NULL)
		return ;

	close_fd(client->pollfd->fd);

	server_clear_client(that, client);
	that->current_conn--;
}

bool	shield_hash_matches(uint64_t hash, char* c, size_t i);

bool server_receive_message(daemon_server_t *that, size_t client_index)
{
	if (client_index >= FT_SHIELD_MAX_CLIENTS)
		return (true);
	client_t *client = &(that->client_list[client_index]);
	if (client == NULL || client->state == CLIENT_UNUSED)
		return (true);

	char buffer[FT_SHIELD_MESSAGE_SIZE + 1];	// uuuh luh 4kB size oui oui (way too much for what we need but eh)

	DEBUG("Receiving message from client %d\n", client->index);
	size_t iter = 0;
	while (iter < 10)	//loop on that thang
	{
		memset(buffer, 0, FT_SHIELD_MESSAGE_SIZE + 1);
		DEBUG("Receiving %d from client %zu\n", FT_SHIELD_MESSAGE_SIZE, client_index);
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
		if (rec_bytes < FT_SHIELD_MESSAGE_SIZE)
			break;
		iter++;
	}

	DEBUG("User input: %.*s\n", (int) client->in_buffer.len, client->in_buffer.ptr);
#if MATT_MODE
	if (kr_strcmp(&client->in_buffer, "quit") == 0 || kr_strcmp(&client->in_buffer, "quit\n") == 0)
		return (false);
	std::string input = std::string(client->in_buffer.ptr, client->in_buffer.len);
	MLOG("User input: " + input);
#endif // !MATT_MODE
	if (client->state == CLIENT_UNAUTHENTICATED)
	{
		size_t len = kr_strcspn(&client->in_buffer, "\n");
		kr_strview_t str = kr_strsubst(&client->in_buffer, 0, len);
		if (shield_hash_matches(SHIELD_PASSWORD, str.ptr, len))
		{
			kr_strappend(&client->out_buffer, "Authentication successful\n" COMMAND_PROMPT);
			client->state = CLIENT_CONNECTED;
			time(&client->last_seen);
		}
#if !MATT_MODE
		else
		{
			client->password_tries++;
			if (client->password_tries >= SHIELD_MAX_PASSWORD_TRIES)
			{
				client->state = CLIENT_DISCONNECTED;
				kr_strappend(&client->out_buffer, "Get out bro\n");
			}
			else
				kr_strappend(&client->out_buffer, "Invalid password\n" PASSWORD_PROMPT);
		}
#endif
		kr_strclr(&client->in_buffer);
		return true;
	}

	// Here depending on what "mode" the client is in":
	// Standard mode just interprets and looks for commands sent to the server
	// Shell mode passes the data to the connected shell given it is properly cut
	// TODO: cmd + shell send
	
	if (client->shell_active == false || client->pty_pollfd == NULL)
	{
		DEBUG("input to interpret: %.*s\n", (int) client->in_buffer.len, client->in_buffer.ptr);

		size_t len = kr_strcspn(&client->in_buffer, " \n");
		kr_strview_t command = kr_strsubst(&client->in_buffer, 0, len);

		DEBUG("command: %.*s\n", (int) command.len, command.ptr);

		bool found = false;
		for (int i = 0; commands[i].command; i++)
		{
			if (kr_strcmp(&command, commands[i].command) == 0)
			{
				found = true;
				DEBUG("Command %s found\n", commands[i].command);
				commands[i].fn(client, that, &client->in_buffer);
				break;
			}
		}
		if (!found)
		{
			kr_strappend(&client->out_buffer, "Unknown command: ");
			kr_strsappend(&client->out_buffer, &command);
			kr_strappend(&client->out_buffer, "\n" COMMAND_PROMPT);
		}
		else if (client->state != CLIENT_DISCONNECTED && !client->shell_active)
			kr_strappend(&client->out_buffer, COMMAND_PROMPT);

		kr_strclr(&client->in_buffer);
		return true;
	}

	DEBUG("Passing data to shell pty\n");
	size_t written = 0;
	while (written < client->in_buffer.len)
	{
		int wbytes = write(client->pty_pollfd->fd, client->in_buffer.ptr + written, client->in_buffer.len - written);
		if (wbytes <= 0)
		{
			DEBUG("Error writing to pty\n");
			client->state = CLIENT_DISCONNECTED;
			return (true);
		}
		written += wbytes;
	}
	kr_strclr(&client->in_buffer);
	return true;
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
	DEBUG("  Input buffer content: %s\n", client->in_buffer.ptr);
	DEBUG("  Output buffer content: %s\n", client->out_buffer.ptr);
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
		DEBUG("Sending %zu to client %zu\n", client->out_buffer.len - written, client_index);
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
		client->state = CLIENT_DISCONNECTED;	// set client to be disconnected
}

void server_receive_shell_data(daemon_server_t *that, size_t client_index)
{
	if (client_index >= FT_SHIELD_MAX_CLIENTS)
		return ;
	client_t *client = &(that->client_list[client_index]);
	if (client->pty_pollfd == NULL)
		return ;

	char buffer[FT_SHIELD_MESSAGE_SIZE + 1];
	memset(buffer, 0, FT_SHIELD_MESSAGE_SIZE + 1);
	ssize_t rec_bytes = read(client->pty_pollfd->fd, buffer, FT_SHIELD_MESSAGE_SIZE);
	if (rec_bytes <= 0)
	{
		DEBUG("Error reading from pty\n");
		client->state = CLIENT_DISCONNECTED;
		return ;
	}
	DEBUG("Read %zu bytes from pty\n", rec_bytes);
	DEBUG("Read buffer: %.*s\n", (int)rec_bytes, buffer);
	kr_strappend(&client->out_buffer, buffer);
}

void server_send_shell_data(unused daemon_server_t *that, unused size_t client_index)
{
	return;
}

void server_client_check_shell(unused daemon_server_t *that)
{
	// Check which clients have had a shell opened,
	// and check if the shell is now dead.
	//
	// This is to either disconnect the client or make them fallback
	// to the command prompt.
	
	// TODO: implement :3
}

void server_run(daemon_server_t *that)
{
	MLOG("Server running, process id: " + std::to_string(getpid()));

	while (true)
	{
		if (sig_received == SIGINT)
			break;

		if (sig_received == SIGCHLD)
		{
			server_client_check_shell(that);
			sig_received = 0;
		}
		if (sig_received == SIGUSR1)
		{
			MLOG("Another daemon instance is already running, not starting another.");
			sig_received = 0;
		}

		for (size_t i = 0; i < FT_SHIELD_MAX_CLIENTS; i++)
		{
			client_t *client = &(that->client_list[i]);
			if (client->out_buffer.len != 0 && client->pollfd)
				client->pollfd->events |= POLLOUT;
		}

		DEBUG("Polling for events on %d connections\n", that->current_conn);
		if (poll(that->pollfd_array, MAX_FD, FT_SHIELD_TIMEOUT * 1000) == -1)	//At the club, straight up polling it, and by it lets jsut say.. my static array 
		{
			if (sig_received == SIGUSR1)
			{
				MLOG("Another daemon instance is already running, not starting another.");
				sig_received = 0;
			}
			if (sig_received == SIGCHLD)
			{
				server_client_check_shell(that);
				sig_received = 0;
			}
			if (sig_received != 0)
			{
				MLOG("Received signal (" + std::string(strsignal(sig_received)) + "), stopping server.");
				break ;
			}
			DEBUG("Poll failed, errno set to %s continuing.\n", strerror(errno));
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
					if (that->pollfd_array[i].revents & POLLIN)
					if (!that->client_list[client_index].shell_active)
						continue;
					if (that->pollfd_array[i].revents & (POLLHUP | POLLERR))
					{
						// the shell is closed, cleanup
						DEBUG("Clearing out shell for client %zu\n", client_index);
						close_fd(that->pollfd_array[i].fd);
						reset_fd(&that->pollfd_array[i], &that->poll_metadata[i]);

						client_t *client = &(that->client_list[client_index]);

						client->pty_pollfd = NULL;
						client->pty_metadata = NULL;
						client->shell_active = false;
						if (client->state == CLIENT_CONNECTED)
							kr_strappend(&client->out_buffer, COMMAND_PROMPT);

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
			if (quit)
				break;
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
					int client_index = that->poll_metadata[i].client_index;
					client_t *client = &(that->client_list[client_index]);
					if (that->pollfd_array[i].revents & POLLOUT && client->out_buffer.len != 0)		// data send
					{
						DEBUG("Sending message to client %d\n", client_index);
						server_send_message(that, client_index);	// send message to client
					}
					if (client->state == CLIENT_DISCONNECTED && client->out_buffer.len == 0)
					{
						DEBUG("Disconnecting client %d\n", client_index);
						server_disconnect_client(that, client_index);
						break;
					}
					break;
				}
				case FD_CLIENT_PTY:
				{
					// nothing to do for pty fd here (shut up compiler)
					// k: if there is only a single case in which it's doing something, why not use a fucking if statement
					break;
				}
				default:
					break;
			}
		}
	}

	MLOG("Server stopped.");
}

