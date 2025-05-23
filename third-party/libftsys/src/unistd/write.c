/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   write.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/10 22:08:36 by kiroussa          #+#    #+#             */
/*   Updated: 2025/05/24 18:49:54 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include <unistd.h>
#include <sys/syscall.h>

ssize_t	write(int fd, const void *buf, size_t count)
{
	if (!buf || fd < 0)
		return (-1);
	return (syscall(SYS_write, fd, buf, count));
}
