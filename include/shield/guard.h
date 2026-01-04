/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   guard.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 22:53:55 by kiroussa          #+#    #+#             */
/*   Updated: 2026/01/04 22:59:38 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __SHIELD_GUARD_H__
#define __SHIELD_GUARD_H__

#ifdef __cplusplus
#define CPPGUARD_START extern "C" {
#define CPPGUARD_END }
#else // !__cplusplus
#define CPPGUARD_START
#define CPPGUARD_END
#endif // __cplusplus

#endif // __SHIELD_GUARD_H__
