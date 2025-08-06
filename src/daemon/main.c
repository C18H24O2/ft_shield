/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lvincent <lvincent@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 03:06:32 by kiroussa          #+#    #+#             */
/*   Updated: 2025/08/06 21:30:36 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <shield/daemon.h>
#include <shield/le_function.h>
#include <stdbool.h>
#include <string.h>
#include <sys/file.h>
#include <unistd.h>

static inline int	shield_daemon_lock(void)
{
	const int	lock_fd = open(DAEMON_LOCK_FILE, O_RDWR | O_CREAT, 0666);

	if (lock_fd < 0)
		return (-1);
	if (flock(lock_fd, LOCK_EX | LOCK_NB) != 0)
	{
		close(lock_fd);
		return (-1);
	}
	int pid = le_getpid();
	(void)!write(lock_fd, &pid, sizeof(pid)); // fuck it, write the bytes
	return (lock_fd);
}

int	shield_daemon_main(void)
{
	const int	lock_fd = shield_daemon_lock();

	if (lock_fd < 0)
		return (1);
	DEBUG("daemon started on pid %d\n", le_getpid());
	shield_daemon_run();
	close(lock_fd);
	unlink(DAEMON_LOCK_FILE);
	return (0);
}
