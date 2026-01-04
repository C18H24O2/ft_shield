/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kr_strnew.cc                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 22:40:31 by kiroussa          #+#    #+#             */
/*   Updated: 2026/01/04 22:51:18 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <shield/string.h>
#include <stdlib.h>

kr_string_t	kr_string_empty = {NULL, 0, 0, false};

kr_string_t	kr_strnew(const char *s)
{
	kr_string_t	str = kr_string_empty;

	if (!s)
		return (str);
	str.len = strlen(s);
	str.ptr = (char *) calloc(1, str.len + 1);
	if (!str.ptr)
		return (str);
	str.owned = true;
	memcpy(str.ptr, s, str.len + 1);
	str.cap = str.len + 1;
	return (str);
}
