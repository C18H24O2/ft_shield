/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unistd.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/11 01:21:57 by kiroussa          #+#    #+#             */
/*   Updated: 2025/11/20 15:12:33 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UNISTD_H
# define UNISTD_H

# ifdef __cplusplus
extern "C" {
# endif // __cplusplus

# define STDIN_FILENO 0
# define STDOUT_FILENO 1
# define STDERR_FILENO 2

# define SEEK_SET 0
# define SEEK_CUR 1
# define SEEK_END 2

# define F_OK 0
# define R_OK 4
# define W_OK 2
# define X_OK 1

# include <stddef.h>
# include <sys/types.h>

extern char	**environ;

long	syscall(long number, ...);

ssize_t	read(int fd, void *buf, size_t count);
ssize_t	write(int fd, const void *buf, size_t count);

int		open(const char *pathname, int flags, ...);
int		close(int fd);

off_t	lseek(int fd, off_t offset, int whence);

# ifdef __cplusplus
} // extern "C"
# endif // __cplusplus

#endif // UNISTD_H
