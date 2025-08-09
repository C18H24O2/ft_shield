/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   le_strsignal.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 21:50:41 by kiroussa          #+#    #+#             */
/*   Updated: 2025/08/09 13:05:46 by kiroussa         ###   ########.fr       */
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

//TODO: replace with libftstd strsignal()
const char	*le_strsignal(int sigval)
{
	const char	*desc_map[] = {
	[SIGHUP] = "Hangup", [SIGINT] = "Interrupt", [SIGQUIT] = "Quit", [SIGILL]
		= "Illegal instruction", [SIGTRAP] = "Trace/breakpoint trap", [SIGABRT]
		= "Aborted", [SIGBUS] = "Bus error", [SIGFPE] = "Floating point "
		"exception", [SIGKILL] = "Killed", [SIGUSR1] = "User defined signal 1",
	[SIGSEGV] = "Segmentation fault", [SIGUSR2] = "User defined signal 2",
	[SIGPIPE] = "Broken pipe", [SIGALRM] = "Alarm clock", [SIGTERM]
		= "Terminated", [SIGTARGET] = SIGTARGET_MSG, [SIGCHLD] = "Child exited",
	[SIGCONT] = "Continued", [SIGSTOP] = "Stopped (signal)", [SIGTSTP]
		= "Stopped", [SIGTTIN] = "Stopped (tty input)", [SIGTTOU]
		= "Stopped (tty output)", [SIGURG] = "Urgent I/O condition", [SIGXCPU]
		= "CPU time limit exceeded", [SIGXFSZ] = "File size limit exceeded",
	[SIGVTALRM] = "Virtual timer expired", [SIGPROF]
		= "Profiling timer expired", [SIGWINCH] = "Window changed", [SIGIO]
		= "I/O possible", [SIGPWR] = "Power failure", [SIGSYS] = "Bad system "
		"call"
	};

	if (sigval < 0)
		return (UNKNOWN_SIGNAL);
	if (sigval > SIGSYS)
		return (RTSIG_MSG);
	return (desc_map[sigval]);
}

