/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memcpy.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/25 16:37:19 by kiroussa          #+#    #+#             */
/*   Updated: 2025/05/25 16:44:19 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>

void	*memcpy(void *dest, const void *src, size_t n)
{
	char	*d;
	char	*s;

	if (!dest || !src)
		return (NULL);
	if (!n)
		return (dest);
	if (dest == src)
		return (dest);
	d = dest;
	s = src;
	while (n--)
		*d++ = *s++;
	return (dest);
}
