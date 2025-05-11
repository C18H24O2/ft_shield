/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stdarg.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/11 01:25:17 by kiroussa          #+#    #+#             */
/*   Updated: 2025/05/11 04:42:50 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STDARG_H
# define STDARG_H

typedef __builtin_va_list	va_list;

# define va_start(v, l) __builtin_va_start(v, l)
# define va_end(v) __builtin_va_end(v)
# define va_arg(v, l) __builtin_va_arg(v, l)

#endif // STDARG_H
