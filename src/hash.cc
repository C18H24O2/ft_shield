/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hash.cc                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 00:47:37 by kiroussa          #+#    #+#             */
/*   Updated: 2026/01/05 11:34:23 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Note: DO NOT rename this file, it's used by the Makefile

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <shield/guard.h>
CPPGUARD_START
#include <nmmintrin.h>
#include <immintrin.h>
CPPGUARD_END

#ifdef HASH_MAIN
#define D(x, ...) fprintf(stderr, x __VA_OPT__(,) __VA_ARGS__)
#else // !HASH_MAIN
# define D(x, ...)
#endif // !HASH_MAIN

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
	D("processing %c at %zu\n", c, i);
	D("base: %llx\n", base);
	if (base == 0xffffffff)
		base = 0x765a5a51;
	base = _mm_crc32_u32((unsigned int) base, c + salt);
	D("got: %llx\n", base);
	if (i % 2 == 0)
		hash = (hash & ~(0xFFFFFFFFull << 8)) | (base << 8);
	else
		hash = (hash & ~(0xFFFFFFFFull << (4 * 8))) | (base << (4 * 8));
	D("hash: %llx\n", hash);
	hash = (hash & ~(0xFFull));
	D("w/o salt: %llx\n", hash);
	hash = (hash | (((uint64_t) salt) & 0xFF));
	D("w/ salt: %llx\n", hash);
	return (hash);
}

bool	shield_hash_matches(uint64_t hash, char* c, size_t i)
{
	int64_t salt = hash & 0xFF;
	uint64_t base = salt | (salt << 8) | (salt << 16) | (salt << 24) | (salt << 32) | (salt << 40) | (salt << 48) | (salt << 56);
	size_t j = 0;
	while (j < i)
	{
		base = shield_hash(base, c[j], j);
		j++;
	}
	return ((base & ~(0xFFull)) | (uint64_t) salt) == hash;
}

#ifdef HASH_MAIN

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

	srand(time(NULL));
	int64_t c = rand() % 256;

	fprintf(stderr, "c: %lld\n", c);

	uint64_t hash = c | (c << 8) | (c << 16) | (c << 24) | (c << 32) | (c << 40) | (c << 48) | (c << 56);
	size_t i = 0;
	while (argv[1][i])
	{
		fprintf(stderr, "=======================\nhash progress: %llx\n", hash);
		hash = shield_hash(hash, argv[1][i], i);
		i++;
	}
	fprintf(stderr, "final: %llx\n", hash);
	printf("%lld", hash);
	return (0);
}

#endif
