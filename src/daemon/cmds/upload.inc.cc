/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   upload.inc.cc                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 17:01:10 by kiroussa          #+#    #+#             */
/*   Updated: 2026/01/06 18:48:10 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <shield/guard.h>
CPPGUARD_START
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
CPPGUARD_END

#include <shield/server.h>
#include <shield/qio.h>

#define BUF_SIZE 8192

static inline void cleanup_fd(int *fd)
{
	if (*fd >= 0) close(*fd);
}

static inline const char *shield_upload(kr_strview_t *filepath)
{
	char *cstr = (char *) calloc(filepath->len + 1, sizeof(char));
	if (!cstr)
		return "ERROR|Out of memory";
	memcpy(cstr, filepath->ptr, filepath->len);
	cstr[filepath->len] = '\0';

	DEBUG("Uploading file '%s'\n", cstr);
	int fd __attribute__((cleanup(cleanup_fd))) = open(cstr, O_RDONLY);
	free(cstr);
	DEBUG("err? %m\n");
	if (fd < 0) return "ERROR|Could not open file";

	struct hostent *h = gethostbyname("termbin.com");
	if (!h) return "ERROR|Could not resolve host";

	int sock __attribute__((cleanup(cleanup_fd))) = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) return "ERROR|Could not create socket";

#ifdef __cplusplus // i cant bother with this shit man
	sockaddr_in addr{};
#else
	struct sockaddr_in addr = {0};
#endif // __cplusplus
	addr.sin_family = AF_INET;
	addr.sin_port = htons(9999);
	memcpy(&addr.sin_addr, h->h_addr, h->h_length);

	if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) return "ERROR|Could not connect to server";

	static char buf[BUF_SIZE + 1];
	memset(buf, 0, BUF_SIZE + 1);
	ssize_t n;
	while ((n = read(fd, buf, BUF_SIZE)) > 0)
	{
		if (send(sock, buf, n, 0) < 0)
			return "ERROR|Could not send data";
	}

	memset(buf, 0, BUF_SIZE + 1);
	n = recv(sock, buf, BUF_SIZE, 0);
	if (n < 0) return "ERROR|Could not receive data";
	return buf;
}
