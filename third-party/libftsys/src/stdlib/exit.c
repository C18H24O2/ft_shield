/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 17:15:31 by kiroussa          #+#    #+#             */
/*   Updated: 2025/05/24 18:10:54 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define _FTSYS_SOURCE
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>

void	exit(int status)
{
	atexit_run_all();
	syscall(SYS_exit, status);
}
