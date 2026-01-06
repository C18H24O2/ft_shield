/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kr_strsubst.cc                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 16:53:13 by kiroussa          #+#    #+#             */
/*   Updated: 2026/01/06 16:33:54 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <shield/string.h>

kr_strview_t	kr_strsubst(kr_string_t *str, size_t pos1, size_t pos2)
{
	if (!str || !str->ptr || pos1 > str->len)
		return (kr_string_empty);
	if (pos1 == 0 && pos2 == str->len)
		return *str;
	if (pos1 > pos2)
		return (kr_string_empty);
	if (pos1 > str->len)
		pos1 = str->len;
	if (pos2 > str->len)
		pos2 = str->len;
	return (kr_strview_t) {
		.ptr = str->ptr + pos1,
		.len = pos2 - pos1,
		.cap = 0,
		.owned = false
	};
}
