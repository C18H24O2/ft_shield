/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 19:51:35 by kiroussa          #+#    #+#             */
/*   Updated: 2026/01/05 00:01:03 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __SHIELD_STRING_H__
#define __SHIELD_STRING_H__

// im not calling ts boxers
// stolen from https://codeberg.org/27/libkroussar

#ifndef __cplusplus
#include <stdbool.h>
#endif // __cplusplus

#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef struct
{
	char *ptr;
	size_t len;
	size_t cap;
	bool owned;
}	kr_string_t;

typedef kr_string_t kr_strview_t;

extern kr_string_t	kr_string_empty;

kr_string_t		kr_strnew(const char *str);
kr_strview_t	kr_strview(const char *str);
void			kr_strdel(kr_string_t *str);
void			kr_strclr(kr_string_t *str);
bool			kr_strgrow(kr_string_t *str, size_t new_cap);

bool			kr_strappend(kr_string_t *str, const char *s);
bool			kr_strsappend(kr_string_t *str, kr_strview_t *s);
kr_strview_t	kr_strsubst(kr_string_t *str, size_t pos1, size_t pos2);

int				kr_strcmp(kr_string_t *str, const char *s);
size_t			kr_strcspn(kr_string_t *str, const char *set);

#endif // __SHIELD_STRING_H__
