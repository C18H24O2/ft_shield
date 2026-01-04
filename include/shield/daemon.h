/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   daemon.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lvincent <lvincent@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 14:20:31 by kiroussa          #+#    #+#             */
/*   Updated: 2026/01/04 22:59:45 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __SHIELD_DAEMON_H__
#define __SHIELD_DAEMON_H__

#if SHIELD_DEBUG
# define DAEMON_LOCK_FILE "/tmp/matt_daemon.lock"
#else
# define DAEMON_LOCK_FILE "/var/lock/matt_daemon.lock"
#endif

void	shield_daemonize(void);
void	shield_daemon_start(void);
int		shield_daemon_main(void);
void	shield_daemon_run(void);

#endif // __SHIELD_DAEMON_H__
