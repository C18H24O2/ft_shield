/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kr_strview.cc                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 00:01:07 by kiroussa          #+#    #+#             */
/*   Updated: 2026/01/05 00:01:30 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <shield/string.h>

kr_strview_t	kr_strview(const char *str)
{
	return (kr_strview_t){.ptr = (char *) str, .len = strlen(str), .cap = 0, .owned = false};
}
