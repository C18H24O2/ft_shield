/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   daemon.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lvincent <lvincent@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 14:20:31 by kiroussa          #+#    #+#             */
/*   Updated: 2025/11/20 09:36:43 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#if SHIELD_DEBUG
# define DAEMON_LOCK_FILE "/tmp/matt_daemon.lock"
#else
# define DAEMON_LOCK_FILE "/var/lock/matt_daemon.lock"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <shield.h>

void	shield_daemonize(void);
void	shield_daemon_start(void);
int		shield_daemon_main(void);
void	shield_daemon_run(void);

#ifdef __cplusplus
}
#endif
