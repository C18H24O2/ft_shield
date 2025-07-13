/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shield.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 02:45:19 by kiroussa          #+#    #+#             */
/*   Updated: 2025/07/13 13:50:02 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHIELD_H
# define SHIELD_H

# ifndef SHIELD_DEBUG
#  define SHIELD_DEBUG 0
#  define DEBUG(fmt, ...)
# elif SHIELD_DEBUG == 1
#  define DEBUG(fmt, ...) printf("[DEBUG] %s: " fmt, __func__ __VA_OPT__(,) __VA_ARGS__)
# endif // !SHIELD_DEBUG

# ifdef __cplusplus
extern "C" {
# endif

int		shield_copy(void);
int		shield_change_entry(int bin);
int		shield_autorun_setup(void);

# ifdef __cplusplus
}
# endif

#endif // SHIELD_H
