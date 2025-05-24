/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   daemon.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lvincent <lvincent@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 14:20:31 by kiroussa          #+#    #+#             */
/*   Updated: 2025/05/24 21:50:00 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void	shield_daemonize(void);
void	shield_daemon_start(void);
int		shield_daemon_main(void);

#ifdef __cplusplus
}
#endif
