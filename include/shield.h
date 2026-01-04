/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shield.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 02:45:19 by kiroussa          #+#    #+#             */
/*   Updated: 2026/01/04 23:01:27 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __SHIELD_H__
#define __SHIELD_H__

#define unused __attribute__((unused))

#ifndef SHIELD_DEBUG
#define SHIELD_DEBUG 0
#define DEBUG(fmt, ...)
#elif SHIELD_DEBUG == 1
#include <stdio.h>
#define DEBUG(fmt, ...) printf("[DEBUG] %s: " fmt, __func__ __VA_OPT__(,) __VA_ARGS__)
#else
#error "Invalid SHIELD_DEBUG value"
#endif // !SHIELD_DEBUG

#define FT_SHIELD_SIGNATURE 0x2B00B135

int		shield_copy(char **binary_path);
int		shield_autorun_setup(const char *binary_path);
int		shield_is_flipped();
int		shield_flip_bit(int fd);
int		shield_path_check(const char *name);

#endif // __SHIELD_H__
