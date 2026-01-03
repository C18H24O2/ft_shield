/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kr_strnew.cc                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 22:40:31 by kiroussa          #+#    #+#             */
/*   Updated: 2026/01/03 22:53:05 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <shield/string.h>
#include <cstdlib>

kr_string_t	kr_strnew(const char *s)
{
	kr_string_t	str;

	memset(&str, 0, sizeof(kr_string_t));
	if (!s)
		return (str);
	str.len = strlen(s);
	str.ptr = (char *) calloc(1, str.len + 1);
	if (!str.ptr)
		return (str);
	str.ptr = (char *)(((uint64_t) str.ptr) | (1ull << 63));
	memcpy(str.ptr, s, str.len + 1);
	str.cap = str.len + 1;
	return (str);
}
