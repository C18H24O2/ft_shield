/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stdlib.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/10 23:40:18 by kiroussa          #+#    #+#             */
/*   Updated: 2025/05/24 18:11:12 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STDLIB_H
# define STDLIB_H

void	atexit(void (*func)(void));
void	exit(int status);

# ifdef _FTSYS_SOURCE

void	atexit_run_all(void);

# endif // _FTSYS_SOURCE

#endif // STDLIB_H
