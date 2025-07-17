/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   antidebug.inc.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 02:42:20 by kiroussa          #+#    #+#             */
/*   Updated: 2025/07/17 02:07:21 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <libgen.h>
#include <limits.h>
#include <shield.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/auxv.h>
#include <sys/ptrace.h>
#include <time.h>
#include <unistd.h>

#ifdef SHIELD_NO_ANTIDEBUG

__attribute__((always_inline))
static inline int	shield_antidebug(void)
{
	return (1);
}

#else

__attribute__((always_inline))
static inline int	shield_check_parent(char *parent)
{
	const char	*name = basename(parent);

	if (!strcmp(name, "gdb") || !strcmp(name, "gdbserver"))
		return (1);
	if (!strcmp(name, "strace"))
		return (1);
	if (!strcmp(name, "ltrace"))
		return (1);
	if (strstr(parent, "lldb"))
		return (1);
	return (0);
}

__attribute__((always_inline))
static inline int	shield_detect_parent(void)
{
	const pid_t	ppid = getppid();
	char		parent[PATH_MAX + 1];
	char		buf[PATH_MAX + 1];
	int			res;

	snprintf(parent, sizeof(parent), "/proc/%d/exe", ppid);
	while (1)
	{
		DEBUG("antidebug/parent readlink of '%s'\n", parent);
		memset(buf, 0, sizeof(buf));
		res = readlink(parent, buf, PATH_MAX);
		if (res < 0)
			break ;
		DEBUG("antidebug/parent readlink res: '%s' %d\n", buf, res);
		memset(parent, 0, sizeof(parent));
		memcpy(parent, buf, res);
	}
	DEBUG("antidebug/parent: %s\n", parent);
	return (shield_check_parent(parent));
}

__attribute__((always_inline))
static inline int	shield_nearheap(void)
{
	static unsigned char	bss;
	const unsigned char		*probe = malloc(0x10);
	int						ret;

	if (!probe)
		return (0);
	ret = !(probe - &bss > 0x20000);
	free((void *)probe);
	DEBUG("antidebug/nearheap ret: %d\n", ret);
	return (ret);
}

__attribute__((always_inline))
static inline int	shield_yeet(void)
{
	int	i;

	i = 0;
	while (i < 2048 * 2048)
		__asm__("int3");
	return (0);
}

__attribute__((always_inline))
static inline int	shield_antidebug(void)
{
	const int		ret = ptrace(PTRACE_TRACEME, 0, 1, 0);
	time_t			start;

	DEBUG("antidebug/ptrace: %d\n", ret);
	if (ret < 0)
		return (shield_yeet());
	ptrace(PTRACE_DETACH, 0, NULL, NULL);
	DEBUG("antidebug/ld_preload: %s\n", getenv("LD_PRELOAD"));
	if (getenv("LD_PRELOAD"))
		return (0);
	start = time(NULL);
	if (shield_detect_parent())
		return (shield_yeet());
	if (shield_nearheap())
		return (0);
	if (time(NULL) - start > 1)
		return (0);
	return (1);
}

#endif
