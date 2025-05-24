/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stdint.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/10 23:53:15 by kiroussa          #+#    #+#             */
/*   Updated: 2025/05/24 19:17:14 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STDINT_H
# define STDINT_H

# include <stddef.h>

typedef unsigned char	uint8_t;
typedef unsigned short	uint16_t;
typedef unsigned int	uint32_t;
typedef unsigned long	uint64_t;
typedef signed char		int8_t;
typedef signed short	int16_t;
typedef signed int		int32_t;
typedef signed long		int64_t;
typedef unsigned int	*uintptr_t;
typedef int				*intptr_t;
typedef long			pid_t;

#endif // STDINT_H
