/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kr_strappend.cc                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 22:53:20 by kiroussa          #+#    #+#             */
/*   Updated: 2026/01/03 23:58:11 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <shield/string.h>

bool	kr_strappend(kr_string_t *str, const char *s)
{
	if (!str)
		return (false);
	if (!s)
		return (true);
	size_t added_len = strlen(s);
	if (added_len == 0)
		return (true);
	size_t new_len = str->len + added_len;
	if (new_len < str->len + 1024)
		new_len = str->len + 1024;
	if (!kr_strgrow(str, new_len))
		return (false);
	if (str->ptr)
	{
		memcpy(str->ptr + str->len, s, added_len);
		str->len += added_len;
	}
	return (true);
}
