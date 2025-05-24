/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   atexit.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 18:02:15 by kiroussa          #+#    #+#             */
/*   Updated: 2025/05/24 18:04:34 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define MAX_ATEXIT_FUNCS 32
static void	(*g_atexit_funcs[MAX_ATEXIT_FUNCS])(void);
static int	g_atexit_count = 0;

void	atexit_run_all(void)
{
	int	i;

	i = 0;
	while (i < g_atexit_count)
		g_atexit_funcs[i++]();
}

int	atexit(void (*func)(void))
{
	if (g_atexit_count >= MAX_ATEXIT_FUNCS)
		return (-1);
	g_atexit_funcs[g_atexit_count++] = func;
	return (0);
}
