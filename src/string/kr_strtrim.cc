/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kr_strtrim.cc                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 13:39:00 by kiroussa          #+#    #+#             */
/*   Updated: 2026/01/05 13:40:37 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <shield/string.h>
#include <ctype.h>

kr_strview_t	kr_strtrim(kr_strview_t *str)
{
	if (!str)
		return (kr_string_empty);
	size_t i = 0;
	while (i < str->len && isspace(str->ptr[i]))
		i++;
	if (i == str->len)
		return (kr_string_empty);
	size_t j = 0;
	while (j < str->len && isspace(str->ptr[str->len - j - 1]))
		j++;
	if (str->len - i - j <= 0)
		return (kr_string_empty);
	return (kr_strview_t) {
		.ptr = str->ptr + i,
		.len = str->len - i - j,
		.cap = 0,
		.owned = false
	};
}
