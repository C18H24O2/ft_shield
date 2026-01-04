/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kr_strsappend.cc                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 23:59:14 by kiroussa          #+#    #+#             */
/*   Updated: 2026/01/05 00:02:12 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <shield/string.h>

bool	kr_strsappend(kr_string_t *str, kr_strview_t *s)
{
	if (!str || !s)
		return (false);
	size_t added_len = s->len;
	if (added_len == 0)
		return (true);
	size_t new_len = str->len + added_len;
	if (new_len < str->len + 1024)
		new_len = str->len + 1024;
	if (!kr_strgrow(str, new_len))
		return (false);
	if (str->ptr)
	{
		memcpy(str->ptr + str->len, s->ptr, added_len);
		str->len += added_len;
	}
	return (true);
}
