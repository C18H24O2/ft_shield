/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   __stack_chk_fail.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/27 21:52:46 by kiroussa          #+#    #+#             */
/*   Updated: 2025/05/30 20:33:53 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <unistd.h>

#define STACK_SMASH	"*** stack smashing detected ***: unspecified error\n"

__attribute__((noreturn))
void	__stack_chk_fail(void)
{
	write(1, STACK_SMASH, sizeof(STACK_SMASH));
	exit(1);
	__builtin_unreachable();
}
