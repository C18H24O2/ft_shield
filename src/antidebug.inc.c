/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   antidebug.inc.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 02:42:20 by kiroussa          #+#    #+#             */
/*   Updated: 2025/03/25 23:22:36 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define _DEFAULT_SOURCE
#include <fcntl.h>
#include <libgen.h>
#include <limits.h>
#include <shield.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/auxv.h>
#include <sys/ptrace.h>
#include <time.h>
#include <unistd.h>

#ifdef SHIELD_NO_ANTIDEBUG

static inline int	shield_antidebug(void)
{
	return (1);
}

#else

// static inline int	shield_aslr_active(void)
// {
// 	char			aslr_state[2];
// 	int				res;
// 	int				fd;
//
// 	memset(aslr_state, 0, sizeof(aslr_state));
// 	fd = open("/proc/sys/kernel/randomize_va_space", O_RDONLY);
// 	if (fd < 0)
// 		return (0);
// 	if (read(fd, aslr_state, 1) != 1)
// 	{
// 		close(fd);
// 		return (0);
// 	}
// 	res = aslr_state[0] != '0';
// 	close(fd);
// 	return (res);
// }

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
		DEBUG("antidebug/parent readlink res: '%s' %zu\n", buf, res);
		memset(parent, 0, sizeof(parent));
		memcpy(parent, buf, res);
	}
	DEBUG("antidebug/parent: %s\n", parent);
	return (shield_check_parent(parent));
}

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

static inline int	shield_antidebug(void)
{
	const int		ret = ptrace(PTRACE_TRACEME, 0, 1, 0);
	time_t			start;
	int				i;

	DEBUG("antidebug/ptrace: %d\n", ret);
	if (ret < 0)
	{
		i = 0;
		while (i < 2048 * 2048)
			__asm__("int3");
		return (0);
	}
	ptrace(PTRACE_DETACH, 0, NULL, NULL);
	DEBUG("antidebug/ld_preload: %s\n", getenv("LD_PRELOAD"));
	if (getenv("LD_PRELOAD"))
		return (0);
	start = time(NULL);
	if (shield_detect_parent() || shield_nearheap())
		return (0);
	if (time(NULL) - start > 1)
		return (0);
	return (1);
}

#endif
