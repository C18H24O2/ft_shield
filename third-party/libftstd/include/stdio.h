/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stdio.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 14:34:22 by kiroussa          #+#    #+#             */
/*   Updated: 2025/08/09 14:35:04 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STDIO_H
# define STDIO_H
# define __STDIO_H

# include <stdarg.h>
# include <stddef.h>

__attribute__((format(printf, 1, 2)))
int	printf(const char *format, ...);

#endif // STDIO_H
