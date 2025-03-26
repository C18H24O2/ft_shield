/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   daemonize.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 14:25:22 by kiroussa          #+#    #+#             */
/*   Updated: 2025/03/25 15:11:45 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <shield/daemon.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static inline void	shield_daemon_setup(void)
{
	int	maxfd;
	int	fd;

	umask(0);
	if (chdir("/") < 0)
		return ;
	maxfd = sysconf(_SC_OPEN_MAX);
	if (maxfd < 0)
		maxfd = 1024;
	fd = 0;
	while (fd < maxfd)
		close(fd++);
	fd = open("/dev/null", O_RDWR);
	if (fd < 0)
		return ;
	if (fd != STDIN_FILENO)
	{
		dup2(fd, STDIN_FILENO);
		close(fd);
	}
	if (dup2(STDIN_FILENO, STDOUT_FILENO) != STDOUT_FILENO)
		return ;
	if (dup2(STDIN_FILENO, STDERR_FILENO) != STDERR_FILENO)
		return ;
}

void	shield_daemonize(void)
{
	pid_t	pid;

	pid = fork();
	if (pid != 0)
		return ;
	if (setsid() < 0)
		return ;
	pid = fork();
	if (pid != 0)
		exit(0);
	shield_daemon_setup();
	shield_daemon_main();
}
