/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shield.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 02:45:19 by kiroussa          #+#    #+#             */
/*   Updated: 2025/03/23 03:30:02 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHIELD_H
# define SHIELD_H

# define DEBUG 1

int		shield_antidebug(void);
int		shield_copy(void);
int		shield_change_entry(int bin);

void	shield_daemon_start(void);
int		shield_daemon_main(void);

#endif // SHIELD_H
