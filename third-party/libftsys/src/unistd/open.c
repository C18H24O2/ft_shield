/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   open.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 18:41:43 by kiroussa          #+#    #+#             */
/*   Updated: 2025/05/24 18:54:05 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <stdarg.h>
#include <sys/syscall.h>
#include <unistd.h>

int	open(const char *filename, int flags, ...)
{
	va_list	ap;
	mode_t	mode;
	int		fd;

	mode = 0;
	if ((flags & O_CREAT) || (flags & O_TMPFILE) == O_TMPFILE)
	{
		va_start(ap, flags);
		mode = va_arg(ap, mode_t);
		va_end(ap);
	}
	fd = syscall(SYS_open, filename, flags, mode);
	if (fd >= 0 && (flags & O_CLOEXEC))
		syscall(SYS_fcntl, fd, F_SETFD, FD_CLOEXEC);
	if (fd < 0)
		return (-1);
	return (fd);
}
