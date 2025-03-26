/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   daemon.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 14:20:31 by kiroussa          #+#    #+#             */
/*   Updated: 2025/03/25 15:17:10 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DAEMON_H
# define DAEMON_H

void	shield_daemonize(void);
void	shield_daemon_start(void);
int		shield_daemon_main(void);

#endif // DAEMON_H
