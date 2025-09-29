/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stdarg.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/11 01:25:17 by kiroussa          #+#    #+#             */
/*   Updated: 2025/09/29 09:25:01 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STDARG_H
# define STDARG_H

# if defined(__builtin_va_list)
typedef __builtin_va_list	va_list;

#  define va_start(v, l) __builtin_va_start(v, l)
#  define va_end(v) __builtin_va_end(v)
#  define va_arg(v, l) __builtin_va_arg(v, l)
# else // !__builtin_va_list
typedef char *	va_list;
# define va_start(ap, parmn) (void)((ap) = (char*)(&(parmn) + 1))
# define va_arg(ap, type) (((type*)((ap) = ((ap) + sizeof(type))))[-1])
# define va_end(ap) (void)((ap) = 0)
#endif // __builtin_va_list

#endif // STDARG_H
