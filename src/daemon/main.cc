/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cc                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lvincent <lvincent@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 03:06:32 by kiroussa          #+#    #+#             */
/*   Updated: 2026/01/11 22:48:11 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <errno.h>
#include <fcntl.h>
#include <shield.h>
#include <shield/daemon.h>
#include <stdbool.h>
#include <sys/file.h>
#include <unistd.h>

static inline int	shield_daemon_lock(void)
{
	const int	lock_fd = open(DAEMON_LOCK_FILE, O_RDWR | O_CREAT, 0644);

	if (lock_fd < 0)
		return (-2);
	if (flock(lock_fd, LOCK_EX | LOCK_UN | LOCK_NB) != 0)
	{
		close(lock_fd);
		return (-3);
	}
	int pid = getpid();
	(void)!write(lock_fd, &pid, sizeof(pid)); // fuck it, write the bytes
	errno = 0;
	return (lock_fd);
}

int	shield_daemon_main(void)
{
	const int	lock_fd = shield_daemon_lock();

	DEBUG("trying to lock on %s, got: %d (%m)\n", DAEMON_LOCK_FILE, lock_fd);
	if (lock_fd < 0)
		return (1);
	DEBUG("daemon started on pid %d\n", getpid());
	shield_daemon_run();
	close(lock_fd);
	unlink(DAEMON_LOCK_FILE);
	return (0);
}
