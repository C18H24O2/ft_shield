/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 18:40:39 by kiroussa          #+#    #+#             */
/*   Updated: 2025/05/24 19:01:10 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include <unistd.h>
#include <sys/syscall.h>

ssize_t	read(int fd, void *buf, size_t count)
{
	if (!buf || fd < 0)
		return (-1);
	return (syscall(SYS_read, fd, buf, count));
}
