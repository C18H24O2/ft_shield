/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   poll.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/20 15:07:31 by kiroussa          #+#    #+#             */
/*   Updated: 2025/11/20 15:10:11 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POLL_H
# define POLL_H

# ifdef __cplusplus
extern "C" {
# endif // __cplusplus

# define POLLIN     0x001
# define POLLPRI    0x002
# define POLLOUT    0x004
# define POLLERR    0x008
# define POLLHUP    0x010
# define POLLNVAL   0x020
# define POLLRDNORM 0x040
# define POLLRDBAND 0x080

# ifndef POLLWRNORM
#  define POLLWRNORM 0x100
#  define POLLWRBAND 0x200
# endif // !POLLWRNORM
//
# ifndef POLLMSG
#  define POLLMSG    0x400
#  define POLLRDHUP  0x2000
# endif // !POLLMSG

struct pollfd
{
	int fd;
	short events;
	short revents;
};

typedef unsigned long nfds_t;

int	poll(struct pollfd *fds, nfds_t nfds, int timeout);

# ifdef __cplusplus
} // extern "C"
# endif // __cplusplus

#endif // POLL_H
