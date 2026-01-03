/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kr_strgrow.cc                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 22:56:19 by kiroussa          #+#    #+#             */
/*   Updated: 2026/01/04 00:16:31 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <shield/string.h>
#include <cstdlib>

bool	kr_strgrow(kr_string_t *str, size_t new_cap)
{
	if (!str || (!str->owned && str->ptr))
		return (false);
	if (new_cap == 0 || new_cap <= str->cap)
		return (true);
	char *new_ptr = (char *) realloc(str->ptr, new_cap);
	if (!new_ptr)
		return (false);
	str->ptr = new_ptr;
	str->owned = true;
	str->cap = new_cap;
	return (true);
}
