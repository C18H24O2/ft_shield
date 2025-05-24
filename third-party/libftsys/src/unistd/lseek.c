/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lseek.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 18:54:31 by kiroussa          #+#    #+#             */
/*   Updated: 2025/05/24 18:54:57 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <sys/syscall.h>

off_t	lseek(int fd, off_t offset, int whence)
{
	return (syscall(SYS_lseek, fd, offset, whence));
}
