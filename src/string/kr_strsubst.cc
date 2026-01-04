/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kr_strsubst.cc                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 16:53:13 by kiroussa          #+#    #+#             */
/*   Updated: 2026/01/04 16:58:23 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <shield/string.h>

kr_string_t	kr_strsubst(kr_string_t *str, size_t pos1, size_t pos2)
{
	if (!str || !str->ptr || pos1 > str->len)
		return (kr_string_empty);
	if (pos2 > str->len)
		pos2 = str->len;
	return (kr_string_t) {
		.ptr = str->ptr + pos1,
		.len = pos2 - pos1,
		.cap = 0,
		.owned = false
	};
}
