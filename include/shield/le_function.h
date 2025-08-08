/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   le_function.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 20:02:54 by kiroussa          #+#    #+#             */
/*   Updated: 2025/08/08 21:52:36 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LE_FUNCTION_H
# define LE_FUNCTION_H

# ifdef __cplusplus
extern "C" {
# endif // __cplusplus

int	le_getpid(void);
const char *le_strsignal(int sigval);

# ifdef __cplusplus
}
# endif // __cplusplus

#endif // LE_FUNCTION_H
