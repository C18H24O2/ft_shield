/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fcntl.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 18:53:03 by kiroussa          #+#    #+#             */
/*   Updated: 2025/05/24 18:53:55 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FCNTL_H
# define FCNTL_H

# define FD_CLOEXEC 1

# define O_RDONLY 0
# define O_WRONLY 1
# define O_RDWR 2
# define O_CREAT        0100
# define O_EXCL         0200
# define O_NOCTTY       0400
# define O_TRUNC       01000
# define O_APPEND      02000
# define O_NONBLOCK    04000
# define O_DSYNC      010000
# define O_SYNC     04010000
# define O_RSYNC    04010000
# define O_DIRECTORY 0200000
# define O_NOFOLLOW  0400000
# define O_CLOEXEC  02000000

# define O_ASYNC      020000
# define O_DIRECT     040000
# define O_LARGEFILE 0100000
# define O_NOATIME  01000000
# define O_PATH    010000000
# define O_TMPFILE 020200000
# define O_NDELAY O_NONBLOCK

# define F_DUPFD  0
# define F_GETFD  1
# define F_SETFD  2
# define F_GETFL  3
# define F_SETFL  4

# define F_SETOWN 8
# define F_GETOWN 9
# define F_SETSIG 10
# define F_GETSIG 11

# define S_ISUID 04000
# define S_ISGID 02000
# define S_ISVTX 01000
# define S_IRUSR 0400
# define S_IWUSR 0200
# define S_IXUSR 0100
# define S_IRWXU 0700
# define S_IRGRP 0040
# define S_IWGRP 0020
# define S_IXGRP 0010
# define S_IRWXG 0070
# define S_IROTH 0004
# define S_IWOTH 0002
# define S_IXOTH 0001
# define S_IRWXO 0007

#endif // FCNTL_H
