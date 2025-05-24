/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   __libc_start_main.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 17:29:53 by kiroussa          #+#    #+#             */
/*   Updated: 2025/05/24 18:29:43 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define _FTSYS_SOURCE
#include <stdlib.h>

char	**environ;

extern void	_init(int argc, char **argv, char **envp) __attribute__((weak));
extern void	_fini(void) __attribute__((weak));

int	__libc_start_main(
    int (*main)(int, char **, char **),
    int argc,
    char **argv,
    int (*unused)(int, char **, char **),
    void (*unused2)(void),
    void (*rtld_fini)(void),
    void *stack_end
) {
	int		result;

	(void)stack_end, (void)unused, (void)unused2;
	environ = argv + argc + 1;
	if (_init)
		_init(argc, argv, environ);
	if (rtld_fini)
		atexit(rtld_fini);
	if (_fini)
		atexit(_fini);
	result = main(argc, argv, environ);
	exit(result);
	return (result);
}
