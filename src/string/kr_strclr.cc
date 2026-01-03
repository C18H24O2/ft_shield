/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kr_strclr.cc                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 22:47:13 by kiroussa          #+#    #+#             */
/*   Updated: 2026/01/03 23:03:47 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <shield/string.h>
#include <cstring>

void	kr_strclr(kr_string_t *str)
{
	if (str)
	{
		str->len = 0;
		memset(str->ptr, 0, str->cap);
	}
}
