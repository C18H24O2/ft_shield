/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   daemon.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lvincent <lvincent@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 14:20:31 by kiroussa          #+#    #+#             */
/*   Updated: 2025/11/29 00:14:11 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#if SHIELD_DEBUG
# define DAEMON_LOCK_FILE "/tmp/matt_daemon.lock"
#else
# define DAEMON_LOCK_FILE "/var/lock/matt_daemon.lock"
#endif

#include <shield.h>

#if !MATT_MODE && __cplusplus
extern "C" {
#endif

void	shield_daemonize(void);
void	shield_daemon_start(void);
int		shield_daemon_main(void);
void	shield_daemon_run(void);

#if !MATT_MODE && __cplusplus
}
#endif
