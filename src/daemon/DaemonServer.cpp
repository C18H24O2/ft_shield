#include <shield/DaemonServer.hpp>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>

DaemonServer::DaemonServer(char password_hash[32]) 
{
	memcpy(_password_hash, password_hash, 32);
	if (FT_SHIELD_MAX_CLIENTS >= 1)
		_client_id = 1;
	else
		_client_id = -1;
}

DaemonServer::~DaemonServer() {}

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
		this->_client_list[0].fd = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol);
		if (this->_client_list[0].fd == -1)
			continue;
		if (setsockopt(this->_client_list[0].fd, SOL_SOCKET, SO_REUSEADDR,&opt, sizeof(opt)) == -1)
		{
			close(this->_client_list[0].fd);
			continue;
		}
		if (bind(this->_client_list[0].fd, tmp->ai_addr, tmp->ai_addrlen) == -1)
		{
			close(this->_client_list[0].fd);
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
	if (fcntl(this->_client_list[0].fd, F_SETFL, O_NONBLOCK) == -1)
	{
		close(this->_client_list[0].fd);
		return 1;
	}
	if (listen(this->_client_list[0].fd, SOMAXCONN) == -1)
	{
		close(this->_client_list[0].fd);
		return 1;
	}

	return 0;
}

void DaemonServer::run()
{

}

