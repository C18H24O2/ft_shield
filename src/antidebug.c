/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   antidebug.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 02:42:20 by kiroussa          #+#    #+#             */
/*   Updated: 2025/03/24 15:20:47 by kiroussa         ###   ########.fr       */
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
	const char	*preload = getenv("LD_PRELOAD");
	int			i;

	if (ret < 0 || preload)
		return (0);
	i = 0;
	while (i < 2048)
		__asm__("int3");
	return (1);
}

#endif
