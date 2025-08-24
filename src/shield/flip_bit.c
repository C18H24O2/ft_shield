/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   flip_bit.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 03:04:47 by kiroussa          #+#    #+#             */
/*   Updated: 2025/08/24 20:06:40 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#if !MATT_MODE

#include <elf.h>
#include <fcntl.h>
#include <shield.h>
#include <shield/daemon.h>
#include <string.h>
#include <unistd.h>

#ifndef ELF_BITNESS
# define ELF_BITNESS 32
# include "flip_bit.c"
# define ELF_BITNESS 64
# include "flip_bit.c"

int	shield_flip_bit(int fd)
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
		return (shield_flip_bit32(fd));
	return (shield_flip_bit64(fd));
}

int	shield_is_flipped()
{
	int			fd;
	Elf32_Ehdr	header;

	fd = open("/proc/self/exe", O_RDONLY);
	if (fd < 0)
		return (0);
	lseek(fd, 0, SEEK_SET);
	if (read(fd, &header, sizeof(header)) != sizeof(header))
	{
		close(fd);
		return (0);
	}
	close(fd);
	if (memcmp(header.e_ident, ELFMAG, SELFMAG))
		return (0);
	if (header.e_ident[EI_CLASS] != ELFCLASS32
		&& header.e_ident[EI_CLASS] != ELFCLASS64)
		return (0);
	lseek(fd, 0, SEEK_SET);
	if (header.e_ident[EI_CLASS] == ELFCLASS32)
		return (shield_is_flipped32(fd));
	return (shield_is_flipped64(fd));
}

#else // ELF_BITNESS
# ifndef ELF_MACROS
#  define ELF_MACROS
#  define _APPEND(a, b) a##b
#  define APPEND(a, b) _APPEND(a, b)

#  define Elf(x) APPEND(APPEND(APPEND(Elf, ELF_BITNESS), _), x)
#  define Func(x) APPEND(x, ELF_BITNESS)
# endif // ELF_MACROS

static inline int	Func(shield_flip_bit)(int fd)
{
	Elf(Ehdr)	header;

	if (read(fd, &header, sizeof(header)) != sizeof(header))
		return (1);
	header.e_flags |= FT_SHIELD_SIGNATURE;
	lseek(fd, 0, SEEK_SET);
	if (write(fd, &header, sizeof(header)) != sizeof(header))
		return (1);
	return (0);
}

static inline int	Func(shield_is_flipped)(int fd)
{
	Elf(Ehdr)	header;

	if (read(fd, &header, sizeof(header)) != sizeof(header))
		return (0);
	return ((header.e_flags & FT_SHIELD_SIGNATURE) == FT_SHIELD_SIGNATURE);
}

# undef ELF_BITNESS
#endif // ELF_BITNESS

#endif // !MATT_MODE
