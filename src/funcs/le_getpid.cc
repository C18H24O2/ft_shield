/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   le_getpid.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 20:04:11 by kiroussa          #+#    #+#             */
/*   Updated: 2025/08/09 13:05:54 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <string.h>
#include <unistd.h>

static inline int	le_atoi(const char *nptr)
{
	long long res = 0;
	int mult = 1;

	while (*nptr == ' ')
		nptr++;
	if (*nptr == '-' || *nptr == '+')
		if (*nptr++ == '-')
			mult = -mult;
	while (*nptr >= '0' && *nptr <= '9')
		res = res * 10 + (*nptr++ - '0');
	return (res * mult);
}

//TODO: fuck this is the wrong pid in daemonization cases, fuck fuck fuck fuck fuck fuck fuck
//TODO: replace with libftsys getpid()
pid_t	le_getpid(void)
{
	const int	fd = open("/proc/self/stat", O_RDONLY);
	char		buf[2049];

	memset(buf, 0, sizeof(buf));
	if (fd < 0)
		return (-1);
	if (read(fd, buf, sizeof(buf) - 1) < 0)
	{
		close(fd);
		return (-1);
	}
	close(fd);

	int sections = 0;
	int i = 0;
	while (sections < 4 && i < (int) (sizeof(buf) - 1))
	{
		if (buf[i] == ' ')
			sections++;
		i++;
	}
	return (le_atoi(buf + i));
}
