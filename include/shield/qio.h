/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   qio.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 18:45:20 by kiroussa          #+#    #+#             */
/*   Updated: 2026/01/04 22:59:08 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __SHIELD_QIO_H__
#define __SHIELD_QIO_H__

#include <stddef.h>
#include <stdint.h>

typedef struct
{
	uint64_t	bytes_received;
	uint64_t	bytes_sent;
	uint64_t	total_connections;
	uint64_t	shells_launched;
}	qio_data_t;

extern qio_data_t	qio_data;

#endif // __SHIELD_QIO_H__
