/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   types.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 18:13:32 by kiroussa          #+#    #+#             */
/*   Updated: 2025/11/20 15:06:20 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SYS_TYPES_H
# define SYS_TYPES_H

#define _Addr long
#define _Int64 long
#define _Reg long

#define __BYTE_ORDER 1234
#define __LONG_MAX 0x7fffffffffffffffL

typedef unsigned _Addr size_t;
typedef unsigned _Addr uintptr_t;
typedef _Addr ptrdiff_t;
typedef _Addr ssize_t;
typedef _Addr intptr_t;
typedef _Addr regoff_t;
typedef _Reg register_t;
typedef _Int64 time_t;
typedef _Int64 suseconds_t;

typedef unsigned mode_t;
typedef unsigned _Reg nlink_t;
typedef _Int64 off_t;
typedef unsigned _Int64 ino_t;
typedef unsigned _Int64 dev_t;
typedef long blksize_t;
typedef _Int64 blkcnt_t;
typedef unsigned _Int64 fsblkcnt_t;
typedef unsigned _Int64 fsfilcnt_t;

typedef unsigned wint_t;
typedef unsigned long wctype_t;

typedef void *timer_t;
typedef int clockid_t;
typedef long clock_t;

struct timeval
{
	time_t tv_sec;
	suseconds_t tv_usec;
};

struct timespec
{
	time_t tv_sec;
	long tv_nsec;
};

typedef int pid_t;
typedef unsigned id_t;
typedef unsigned uid_t;
typedef unsigned gid_t;
typedef int key_t;
typedef unsigned useconds_t;

#endif // SYS_TYPES_H
