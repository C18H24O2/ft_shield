/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   le_strsignal.cc                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 21:50:41 by kiroussa          #+#    #+#             */
/*   Updated: 2025/11/29 00:13:11 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <signal.h>

#define UNKNOWN_SIGNAL "Unknown signal"
#define RTSIG_MSG "Real-time signal (idk which one lol)"

#if defined(SIGSTKFLT)
# define SIGTARGET SIGSTKFLT
# define SIGTARGET_MSG "Stack fault"
#elif defined(SIGEMT)
# define SIGTARGET SIGEMT
# define SIGTARGET_MSG "Emulator trap"
#else
# define SIGTARGET 16
# define SIGTARGET_MSG "Unknown signal"
#endif

#ifndef SIGRTMIN
# define SIGRTMIN 0
#endif

struct sigdef
{
	int	sig;
	const char	*name;
};

static struct sigdef sigs[] = {
	{SIGHUP, "Hangup"}, {SIGINT, "Interrupt"}, {SIGQUIT, "Quit"}, {SIGILL, "Illegal instruction"},
	{SIGTRAP, "Trace/breakpoint trap"}, {SIGABRT, "Aborted"}, {SIGBUS, "Bus error"},
	{SIGFPE, "Floating point exception"}, {SIGKILL, "Killed"}, {SIGUSR1, "User defined signal 1"},
	{SIGSEGV, "Segmentation fault"}, {SIGUSR2, "User defined signal 2"}, {SIGPIPE, "Broken pipe"},
	{SIGALRM, "Alarm clock"}, {SIGTERM, "Terminated"}, {SIGSTKFLT, "Stack fault"},
	{SIGCHLD, "Child exited"}, {SIGCONT, "Continued"}, {SIGSTOP, "Stopped (signal)"},
	{SIGTSTP, "Stopped"}, {SIGTTIN, "Stopped (tty input)"}, {SIGTTOU, "Stopped (tty output)"},
	{SIGURG, "Urgent I/O condition"}, {SIGXCPU, "CPU time limit exceeded"},
	{SIGXFSZ, "File size limit exceeded"}, {SIGVTALRM, "Virtual timer expired"},
	{SIGPROF, "Profiling timer expired"}, {SIGWINCH, "Window changed"}, {SIGIO, "I/O possible"},
	{SIGPWR, "Power failure"}, {SIGSYS, "Bad system call"}
};

//TODO: replace with libftstd strsignal()
const char	*le_strsignal(int sigval)
{
	if (sigval < 0)
		return (UNKNOWN_SIGNAL);
	if (sigval > SIGSYS)
		return (RTSIG_MSG);
	for (int i = 0; i < (int)(sizeof(sigs) / sizeof(struct sigdef)); i++)
	{
		if (sigs[i].sig == sigval)
			return (sigs[i].name);
	}
	return (UNKNOWN_SIGNAL);
}

