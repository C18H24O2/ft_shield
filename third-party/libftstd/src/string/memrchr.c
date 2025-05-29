/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memrchr.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/25 16:45:20 by kiroussa          #+#    #+#             */
/*   Updated: 2025/05/25 16:46:44 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>

void	*memrchr(const void *s, int c, size_t n)
{
	unsigned char	*p;

	if (!s || !n)
		return (NULL);
	p = s + n;
	while (n--)
		if (*--p == (unsigned char) c)
			return ((void *) p);
	return (NULL);
}
