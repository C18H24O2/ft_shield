/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   daemon.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lvincent <lvincent@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 14:20:31 by kiroussa          #+#    #+#             */
/*   Updated: 2025/07/17 01:23:36 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#define DAEMON_LOCK_FILE "/var/lock/matt_daemon.lock"

#ifdef __cplusplus
extern "C" {
#endif

void	shield_daemonize(void);
void	shield_daemon_start(void);
int		shield_daemon_main(void);
void	shield_daemon_run(void);

#ifdef __cplusplus
}
#endif
