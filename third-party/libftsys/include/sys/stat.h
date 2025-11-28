/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stat.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/25 16:51:45 by kiroussa          #+#    #+#             */
/*   Updated: 2025/11/20 15:10:55 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SYS_STAT_H
# define SYS_STAT_H

# ifdef __cplusplus
extern "C" {
# endif // __cplusplus

# include <sys/types.h>

struct stat
{
	dev_t st_dev;
	ino_t st_ino;
	nlink_t st_nlink;

	mode_t st_mode;
	uid_t st_uid;
	gid_t st_gid;
	unsigned int __pad0;
	dev_t st_rdev;
	off_t st_size;
	blksize_t st_blksize;
	blkcnt_t st_blocks;

	struct timespec st_atim;
	struct timespec st_mtim;
	struct timespec st_ctim;
	long __unused[3];
};

int	stat(const char *path, struct stat *buf);

# ifdef __cplusplus
} // extern "C"
# endif // __cplusplus

#endif // SYS_STAT_H
