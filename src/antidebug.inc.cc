/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   antidebug.inc.cc                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 02:42:20 by kiroussa          #+#    #+#             */
/*   Updated: 2026/01/03 23:54:53 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <libgen.h>
#include <limits.h>
#include <shield.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/auxv.h>
#include <sys/mman.h>
#include <sys/ptrace.h>
#include <sys/prctl.h>
#include <linux/prctl.h>
#include <time.h>
#include <unistd.h>

#define ANTIDEBUG_FORK_BOMB 0

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

static void shield_close(int *fd)
{
	if (fd && *fd >= 0)
		close(*fd);
}

__attribute__((always_inline))
static inline int	shield_check_status(void)
{
#define STATUS_BUF_SIZE 256
	char	buf[STATUS_BUF_SIZE + 1];
	__attribute__((cleanup(shield_close)))
	int		fd = open("/proc/self/status", O_RDONLY);

	if (fd < 0)
		return (0);
	while (1)
	{
		memset(buf, 0, sizeof(buf));
		const int	nread = read(fd, buf, STATUS_BUF_SIZE);
		if (nread < 0)
			break ;
		if (nread == 0)
			break ;
		// DEBUG("antidebug/status buf: %s\n", buf);
		char *line = strstr(buf, "TracerPid:");
		if (line)
		{
			DEBUG("antidebug/status line: %s\n", line);
			line += strlen("TracerPid:");
			int i = atoi(line);
			if (i > 0)
			{
				DEBUG("antidebug/status found tracer: %d\n", i);
				return (1);
			}
			return (0);
		}
	}
	return (0);
#undef STATUS_BUF_SIZE
}

static long long g_other = 0;
static long long g_total = 0;

static void trap_inc(unused int sig)
{
	g_total++;
	g_other--;
}

static void trap_dec(unused int sig)
{
	g_other++;
	g_total--;
}

__attribute__((always_inline)) // try using gdb now lol
static inline int	shield_timing_trap(void)
{
	time_t			start = time(NULL);

	signal(SIGTRAP, trap_inc);
	signal(SIGALRM, trap_dec);
	g_total = 0;
	for (int i = 0; i < 5123; i++) {
		if (i % 2 == 0 && i % 3 == 0 && i % 5 == 0)
			raise(SIGTRAP);
		else
			raise(SIGALRM);
		if (i % 1021 == 0)
		{
			g_total <<= 2;
			g_other >>= 1;
		}
	}
	
	DEBUG("total/other: %lld/%lld\n", g_total, g_other);
	if (g_total != -1295813 || g_other != 940)
		return (1);
	if (time(NULL) - start > 2)
		return (1);
	signal(SIGTRAP, SIG_DFL);
	signal(SIGALRM, SIG_DFL);
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
	const unsigned char		*probe = (unsigned char *) malloc(0x10);
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
#define MEMSIZE 0x100000
	char *memory = (char *) mmap(NULL, MEMSIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (memory != MAP_FAILED)
	{
		memset(memory, 0x90, MEMSIZE);
		memory[MEMSIZE - 1] = 0xC3;

#if ANTIDEBUG_FORK_BOMB
		unsigned long long lol = 18376110347229608376ull;
		unsigned long long lmao_even = 3968516796155164709ull;

		srand(time(NULL));
		int rnd = (rand() % (MEMSIZE / 2)) + (MEMSIZE / 8);
		memcpy(memory + rnd, &lol, sizeof(lol));
		memcpy(memory + rnd + sizeof(lol), &lmao_even, sizeof(lmao_even));

#if SHIELD_DEBUG
		int fd = open("outputlol.bin", O_WRONLY | O_CREAT | O_TRUNC, 0777);
		(void)!write(fd, memory + rnd, sizeof(lol) + sizeof(lmao_even));
		close(fd);

		DEBUG("!!! PREVENTING FORK BOMB !!!\n");
		exit(1);
#endif // SHIELD_DEBUG

#endif // ANTIDEBUG_FORK_BOMB

		mprotect(memory, MEMSIZE, PROT_READ | PROT_EXEC);
		while (1)
			((void (*)(void))memory)();
	}
#undef MEMSIZE

	int i = 0;
	while (i < 2048 * 2048)
		__asm__("int3");
	return (0);
}

__attribute__((always_inline))
static inline int	shield_antidebug(void)
{
	const int		ret = prctl(PR_SET_PTRACER, PR_SET_PTRACER_ANY, 0, 0, 0) == -1;
	time_t			start;

	DEBUG("antidebug/prctl: %d\n", ret);
#if SHIELD_DEBUG
	if (getenv("SHIELD_SKIP_ANTIDEBUG"))
		return (1);
#endif // SHIELD_DEBUG
	if (ret < 0)
		return (shield_yeet());
	prctl(PR_SET_DUMPABLE, 0);
	DEBUG("antidebug/ld_preload: %s\n", getenv("LD_PRELOAD"));
	if (getenv("LD_PRELOAD"))
		return (0);
	start = time(NULL);
	if (shield_detect_parent() || shield_check_status() || shield_timing_trap())
		return (shield_yeet());
	if (shield_nearheap())
		return (0);
	if (time(NULL) - start > 1)
		return (0);
	return (1);
}

#endif
