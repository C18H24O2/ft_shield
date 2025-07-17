/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lvincent <lvincent@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 03:06:32 by kiroussa          #+#    #+#             */
/*   Updated: 2025/07/17 01:18:25 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <shield/daemon.h>
#include <stdbool.h>
#include <sys/file.h>
#include <unistd.h>

#if !SHIELD_DEBUG

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
	return (lock_fd);
}

#endif // !SHIELD_DEBUG

int	shield_daemon_main(void)
{
#if !SHIELD_DEBUG
	const int	lock_fd = shield_daemon_lock();

	if (lock_fd < 0)
		return (1);
#endif // !SHIELD_DEBUG
	shield_daemon_run();
#if !SHIELD_DEBUG
	close(lock_fd);
	unlink(DAEMON_LOCK_FILE);
#endif // !SHIELD_DEBUG
	return (0);
}
