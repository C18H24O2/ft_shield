/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 21:46:27 by kiroussa          #+#    #+#             */
/*   Updated: 2025/05/25 16:36:49 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRING_H
# define STRING_H

# include <stddef.h>

size_t	strlen(const char *s);

void	*bzero(void *s, size_t n);
void	*memcpy(void *dest, const void *src, size_t n);
void	*memccpy(void *dest, const void *src, int c, size_t n);
void	*memset(void *s, int c, size_t n);
void	*memmove(void *dest, const void *src, size_t n);
void	*memchr(const void *s, int c, size_t n);
void	*memrchr(const void *s, int c, size_t n);
int		memcmp(const void *s1, const void *s2, size_t n);

#endif // STRING_H
