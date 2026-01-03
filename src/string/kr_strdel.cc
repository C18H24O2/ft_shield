/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kr_strdel.cc                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 22:46:22 by kiroussa          #+#    #+#             */
/*   Updated: 2026/01/03 22:52:24 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <shield/string.h>
#include <cstdlib>

void	kr_strdel(kr_string_t *str)
{
	if (!str)
		return ;
	if (str->ptr && kr_strowns(str))
		free(str->ptr);
	str->ptr = NULL;
	str->len = 0;
	str->cap = 0;
}
