/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   daemonize.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 14:25:22 by kiroussa          #+#    #+#             */
/*   Updated: 2025/08/06 18:09:58 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <shield/daemon.h>

#if SHIELD_DEBUG
# define SHIELD_SKIP_DAEMONIZE 1
#else // !SHIELD_DEBUG
# define SHIELD_SKIP_DAEMONIZE 0
#endif // SHIELD_DEBUG

#if !SHIELD_SKIP_DAEMONIZE
#include <fcntl.h>
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
	// shut the fuck up dawg
	if (dup2(STDIN_FILENO, STDOUT_FILENO) != STDOUT_FILENO)
		return ;
	if (dup2(STDIN_FILENO, STDERR_FILENO) != STDERR_FILENO)
		return ;
}
#endif // !SHIELD_SKIP_DAEMONIZE

void	shield_daemonize(void)
{
#if !SHIELD_SKIP_DAEMONIZE
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
#endif // !SHIELD_SKIP_DAEMONIZE
	shield_daemon_main();
}
