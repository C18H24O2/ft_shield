/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   change_entry.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 03:04:47 by kiroussa          #+#    #+#             */
/*   Updated: 2025/05/24 19:55:38 by kiroussa         ###   ########.fr       */
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
#else // ELF_BITNESS
# define _APPEND(a, b) a##b
# define APPEND(a, b) _APPEND(a, b)
# define Elf(x) APPEND(APPEND(APPEND(Elf, ELF_BITNESS), _), x)

static inline int	shield_change_entry32(int bin)
{
	Elf32_Ehdr	header;
	long long	offset;

	if (read(bin, &header, sizeof(header)) != sizeof(header))
		return (1);
	offset = (unsigned long long) & _start
		- (unsigned long long)& shield_daemon_start;
	header.e_entry -= offset;
	lseek(bin, 0, SEEK_SET);
	if (write(bin, &header, sizeof(header)) != sizeof(header))
		return (1);
	return (0);
}

static inline int	shield_change_entry64(int bin)
{
	Elf64_Ehdr	header;
	long long	offset;

	if (read(bin, &header, sizeof(header)) != sizeof(header))
		return (1);
	offset = (unsigned long long) & _start
		- (unsigned long long)& shield_daemon_start;
	header.e_entry -= offset;
	lseek(bin, 0, SEEK_SET);
	if (write(bin, &header, sizeof(header)) != sizeof(header))
		return (1);
	return (0);
}

int	shield_change_entry(int bin)
{
	Elf32_Ehdr	header;

	lseek(bin, 0, SEEK_SET);
	if (read(bin, &header, sizeof(header)) != sizeof(header))
		return (1);
	if (memcmp(header.e_ident, ELFMAG, SELFMAG))
		return (1);
	if (header.e_ident[EI_CLASS] != ELFCLASS32
		&& header.e_ident[EI_CLASS] != ELFCLASS64)
		return (1);
	lseek(bin, 0, SEEK_SET);
	if (header.e_ident[EI_CLASS] == ELFCLASS32)
		return (shield_change_entry32(bin));
	return (shield_change_entry64(bin));
}
