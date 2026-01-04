/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kr_strcspn.cc                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 16:29:15 by kiroussa          #+#    #+#             */
/*   Updated: 2026/01/04 16:29:30 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <shield/string.h>

size_t	kr_strcspn(kr_string_t *str, const char *set)
{
	size_t	i;
	size_t	len;

	if (!str || !set)
		return (0);
	len = str->len;
	for (i = 0; i < len; i++)
	{
		if (strchr(set, str->ptr[i]))
			return (i);
	}
	return (len);
}
