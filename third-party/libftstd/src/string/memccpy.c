/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memccpy.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/25 16:38:08 by kiroussa          #+#    #+#             */
/*   Updated: 2025/05/25 16:44:52 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>

void	*memccpy(void *dest, const void *src, int c, size_t n)
{
	unsigned char	*d;
	unsigned char	*s;

	if (!dest || !src || !n)
		return (NULL);
	d = dest;
	s = src;
	while (n--)
	{
		*d = *s;
		if (*d == (unsigned char) c)
			return (d);
		d++;
		s++;
	}
	return (NULL);
}
