/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unistd.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/11 01:21:57 by kiroussa          #+#    #+#             */
/*   Updated: 2025/05/11 04:42:21 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UNISTD_H
# define UNISTD_H

# define STDIN_FILENO 0
# define STDOUT_FILENO 1
# define STDERR_FILENO 2

# define SEEK_SET 0
# define SEEK_CUR 1
# define SEEK_END 2

# define O_RDONLY 0
# define O_WRONLY 1
# define O_RDWR 2
# define O_CREAT 0100
# define O_EXCL 0200
# define O_TRUNC 0400
# define O_APPEND 01000
# define O_NONBLOCK 02000

# define F_OK 0
# define R_OK 4
# define W_OK 2
# define X_OK 1

# include <stddef.h>

long	syscall(long number, ...);
ssize_t	write(int fd, const void *buf, size_t count);

#endif // UNISTD_H
