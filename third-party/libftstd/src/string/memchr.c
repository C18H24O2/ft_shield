/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memchr.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/25 16:45:10 by kiroussa          #+#    #+#             */
/*   Updated: 2025/05/25 16:46:54 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>

void	*memchr(const void *s, int c, size_t n)
{
	unsigned char	*p;

	if (!s || !n)
		return (NULL);
	p = s;
	while (n--)
		if (*p++ == (unsigned char) c)
			return ((void *) p - 1);
	return (NULL);
}
