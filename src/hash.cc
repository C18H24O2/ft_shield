/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hash.cc                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 00:47:37 by kiroussa          #+#    #+#             */
/*   Updated: 2025/11/29 01:48:00 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Note: DO NOT rename this file, it's used by the Makefile

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <nmmintrin.h>
#include <immintrin.h>

// 0x FF FF FF FF FF FF FF FF
//    ^^^^^^^^^^^
//      part 1
//
// 0x FF FF FF FF FF FF FF FF
//             ^^^^^^^^^^^
//               part 2
//
// 0x FF FF FF FF FF FF FF FF
//                         ^^
//                        salt

uint64_t	shield_hash(uint64_t hash, int8_t c, size_t i)
{
	int8_t salt = hash & 0xFF;

	uint64_t base;
	if (i % 2 == 0)
		base = (hash >> 8) & 0xFFFFFFFF;
	else
		base = (hash >> (4 * 8) & 0xFFFFFFFF);
	base = _mm_crc32_u32((unsigned int) base, c + salt);
	if (i % 2 == 0)
		hash = (hash & ~(0xFFFFFFFFll << 8)) | (base << 8);
	else
		hash = (hash & ~(0xFFFFFFFFll << (4 * 8))) | (base << (4 * 8));
	return (hash);
}

#ifdef HASH_MAIN

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s <string>\n", argv[0]);
		return (1);
	}

	int64_t c;
	uint16_t tmp;
	if (_rdrand16_step(&tmp))
		c = tmp;
	else
	{
		srand(time(NULL));
		c = rand() % 256;
	}

	uint64_t hash = c | (c << 8) | (c << 16) | (c << 24) | (c << 32) | (c << 40) | (c << 48) | (c << 56);
	size_t i = 0;
	while (argv[1][i])
	{
		hash = shield_hash(hash, argv[1][i], i);
		i++;
	}
	printf("%llx", hash);
	return (0);
}

#endif
