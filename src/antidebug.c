/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   antidebug.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 02:42:20 by kiroussa          #+#    #+#             */
/*   Updated: 2025/03/23 03:31:41 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <shield.h>
#include <sys/ptrace.h>

#if DEBUG

int	shield_antidebug(void)
{
	return (1);
}

#else

int	shield_antidebug(void)
{
	const int	ret = ptrace(PTRACE_TRACEME, 0, 1, 0);

	if (ret < 0)
		return (0);
	return (1);
}

#endif
