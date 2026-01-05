/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kr_strspn.cc                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 12:01:54 by kiroussa          #+#    #+#             */
/*   Updated: 2026/01/05 12:03:34 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <shield/string.h>

size_t	kr_strspn(kr_strview_t *str, const char *accept)
{
	if (!str || !accept)
		return (0);
	size_t i = 0;
	while (i < str->len && strchr(accept, str->ptr[i]))
		i++;
	if (i >= str->len)
		return (str->len);
	return (i);
}
