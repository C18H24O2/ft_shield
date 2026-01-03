/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   qio.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 18:45:20 by kiroussa          #+#    #+#             */
/*   Updated: 2026/01/03 18:46:34 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

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
