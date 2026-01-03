/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kr_strcmp.cc                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 23:09:53 by kiroussa          #+#    #+#             */
/*   Updated: 2026/01/03 23:15:04 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <shield/string.h>

int kr_strcmp(kr_string_t *str, const char *s)
{
	if (!str || !s)
		return (-1);
	size_t len = strlen(s);
	size_t min_len = len;
	if (str->len < len)
		min_len = str->len; 
	for (size_t i = 0; i < min_len; i++)
	{
		if (str->ptr[i] != s[i])
			return (unsigned char)str->ptr[i] - (unsigned char)s[i];
	}
	if (str->len < len)
		return 0 - (unsigned char)s[str->len];
	else if (str->len > len)
		return (unsigned char)str->ptr[len];
	return 0;
}
