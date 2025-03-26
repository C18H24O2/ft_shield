/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 03:06:32 by kiroussa          #+#    #+#             */
/*   Updated: 2025/03/25 16:00:48 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <shield/daemon.h>
#include <stdbool.h>
#include <sys/file.h>
#include <unistd.h>

#define LOCK_FILE "/tmp/shield.lock"

static inline int	shield_daemon_lock(void)
{
	const int	lock_fd = open(LOCK_FILE, O_RDWR | O_CREAT, 0666);

	if (lock_fd < 0)
		return (-1);
	if (flock(lock_fd, LOCK_EX | LOCK_NB) != 0)
	{
		close(lock_fd);
		return (-1);
	}
	return (lock_fd);
}

int	shield_daemon_main(void)
{
	const int	lock_fd = shield_daemon_lock();

	if (lock_fd < 0)
		return (1);
	while (true)
		pause();
	close(lock_fd);
	unlink(LOCK_FILE);
	return (0);
}
