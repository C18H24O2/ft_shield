/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   upload.inc.cc                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 17:01:10 by kiroussa          #+#    #+#             */
/*   Updated: 2026/01/07 12:20:27 by kiroussa         ###   ########.fr       */
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

static inline void cleanup_fd(int *fd)
{
	if (*fd >= 0) close(*fd);
}

// #if SHIELD_DEBUG
// #define CDEBUG(x) kr_strappend(&client->out_buffer, x)
// #else // !SHIELD_DEBUG
// # define CDEBUG(x)
// #endif // !SHIELD_DEBUG

static const char base64_table[] = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

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

	ssize_t n;
#define BUF_SIZE 1024*3
	static char buffer[BUF_SIZE];
	static char b64buffer[BUF_SIZE * 2];
	while ((n = read(fd, buffer, BUF_SIZE)) > 0)
	{
		DEBUG("Read %zu bytes\n", n);
		memset(b64buffer, 0, sizeof(b64buffer));
		size_t j = 0;
		for (size_t i = 0; i < (size_t)n; i += 3, j += 4)
		{
			unsigned char a = buffer[i];
			unsigned char b = (i + 1 < (size_t)n) ? buffer[i + 1] : 0;
			unsigned char c = (i + 2 < (size_t)n) ? buffer[i + 2] : 0;
			DEBUG("writing from %zu to %zu\n", j, j + 4);
			b64buffer[j] = base64_table[a >> 2];
			b64buffer[j + 1] = base64_table[((a & 3) << 4) | (b >> 4)];
			b64buffer[j + 2] = (i + 1 < (size_t)n) ? base64_table[((b & 0xF) << 2) | (c >> 6)] : '=';
			b64buffer[j + 3] = (i + 2 < (size_t)n) ? base64_table[c & 0x3F] : '=';
		}
		DEBUG("b64 len: %zu\n", j);
		DEBUG("b64: '%.*s'\n", (int) j, b64buffer);
		size_t total_sent = 0;
		while (total_sent < (size_t)j)
		{
			ssize_t sent = send(sock, b64buffer + total_sent, j - total_sent, 0);
			DEBUG("Sent %zd bytes\n", sent);
			if (sent < 0)
				return "ERROR|Could not send data";
			total_sent += sent;
		}
	}
	DEBUG("done sending\n");

	memset(buffer, 0, sizeof(buffer));
	n = recv(sock, buffer, BUF_SIZE, 0);
	if (n < 0) return "ERROR|Could not receive data";
	buffer[n] = '\0';
	DEBUG("Received: %s\n", buffer);
	return buffer;
}
