/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memmove.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/25 16:42:52 by kiroussa          #+#    #+#             */
/*   Updated: 2025/05/25 16:44:04 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>

void	*memmove(void *dest, const void *src, size_t n)
{
	unsigned char	*d;
	unsigned char	*s;

	if (!dest || !src)
		return (NULL);
	if (!n)
		return (dest);
	if (dest == src)
		return (dest);
	d = dest;
	s = src;
	if (d < s)
		while (n--)
			*d++ = *s++;
	else
		while (n--)
			*--d = *--s;
	return (dest);
}
