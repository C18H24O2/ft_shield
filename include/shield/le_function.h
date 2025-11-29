/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   le_function.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 20:02:54 by kiroussa          #+#    #+#             */
/*   Updated: 2025/11/29 01:53:00 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LE_FUNCTION_H
# define LE_FUNCTION_H

#if !MATT_MODE && __cplusplus
extern "C" {
#endif

int	le_getpid(void);
const char *le_strsignal(int sigval);

#if !MATT_MODE && __cplusplus
}
#endif

#endif // LE_FUNCTION_H
