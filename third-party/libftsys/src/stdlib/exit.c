/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 17:15:31 by kiroussa          #+#    #+#             */
/*   Updated: 2025/05/27 21:47:49 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define _FTSYS_SOURCE
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>

void	_exit(int status)
{
	syscall(SYS_exit, status);
}

void	exit(int status)
{
	atexit_run_all();
	_exit(status);
}

void	_Exit(int status)
		__attribute__((weak, alias("_exit")));
