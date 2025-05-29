/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   change_entry.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 03:04:47 by kiroussa          #+#    #+#             */
/*   Updated: 2025/05/25 16:34:16 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <elf.h>
#include <shield.h>
#include <shield/daemon.h>
#include <string.h>
#include <unistd.h>

void	_start(void);

#ifndef ELF_BITNESS
# define ELF_BITNESS 32
# include "change_entry.c"
# define ELF_BITNESS 64
# include "change_entry.c"

int	shield_change_entry(int fd)
{
	Elf32_Ehdr	header;

	lseek(fd, 0, SEEK_SET);
	if (read(fd, &header, sizeof(header)) != sizeof(header))
		return (1);
	if (memcmp(header.e_ident, ELFMAG, SELFMAG))
		return (1);
	if (header.e_ident[EI_CLASS] != ELFCLASS32
		&& header.e_ident[EI_CLASS] != ELFCLASS64)
		return (1);
	lseek(fd, 0, SEEK_SET);
	if (header.e_ident[EI_CLASS] == ELFCLASS32)
		return (shield_change_entry32(fd));
	return (shield_change_entry64(fd));
}

#else // ELF_BITNESS
# ifndef ELF_MACROS
#  define ELF_MACROS
#  define _APPEND(a, b) a##b
#  define APPEND(a, b) _APPEND(a, b)

#  define Elf(x) APPEND(APPEND(APPEND(Elf, ELF_BITNESS), _), x)
#  define Func(x) APPEND(x, ELF_BITNESS)
# endif // ELF_MACROS

static inline int	Func(shield_change_entry)(int fd)
{
	Elf(Ehdr)	header;
	long long	offset;

	if (read(fd, &header, sizeof(header)) != sizeof(header))
		return (1);
	offset = (unsigned long long) & _start
		- (unsigned long long)& shield_daemon_start;
	header.e_entry -= offset;
	lseek(fd, 0, SEEK_SET);
	if (write(fd, &header, sizeof(header)) != sizeof(header))
		return (1);
	return (0);
}

# undef ELF_BITNESS
#endif // ELF_BITNESS
