/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 19:51:35 by kiroussa          #+#    #+#             */
/*   Updated: 2026/01/04 00:15:59 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// im not calling ts boxers
// stolen from https://codeberg.org/27/libkroussar

#ifndef __cplusplus
#include <stdbool.h>
#endif // __cplusplus

#include <stddef.h>
#include <stdint.h>
#include <cstring>

typedef struct
{
	char *ptr;
	size_t len;
	size_t cap;
	bool owned;
}	kr_string_t;

kr_string_t	kr_strnew(const char *str);
void		kr_strdel(kr_string_t *str);
void		kr_strclr(kr_string_t *str);

bool		kr_strgrow(kr_string_t *str, size_t new_cap);
bool		kr_strappend(kr_string_t *str, const char *s);

int			kr_strcmp(kr_string_t *str, const char *s);
