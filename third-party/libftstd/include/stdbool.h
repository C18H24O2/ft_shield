/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stdbool.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/25 16:49:18 by kiroussa          #+#    #+#             */
/*   Updated: 2025/05/25 16:50:58 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STDBOOL_H
# define STDBOOL_H
# define __STDBOOL_H

# define __bool_true_false_are_defined 1

# if defined(__STDC_VERSION__) && __STDC_VERSION__ > 201710L
/* C23 defines true and false. */
# elif !defined(__cplusplus)
#  define bool _Bool
#  define true 1
#  define false 0
# elif defined(__GNUC__) && !defined(__STRICT_ANSI__)
/* Define _Bool as a GNU extension. */
#  define _Bool bool
#  if defined(__cplusplus) && __cplusplus < 201103L

/* For C++98, define bool, false, true as a GNU extension. */
#   define bool bool
#   define false false
#   define true true
#  endif /* __cplusplus < 201103L */
# endif /* __GNUC__ && !__STRICT_ANSI__ */

#endif /* STDBOOL_H */
