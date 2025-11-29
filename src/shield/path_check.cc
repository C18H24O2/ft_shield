/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_check.cc                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/28 23:17:58 by kiroussa          #+#    #+#             */
/*   Updated: 2025/11/29 00:13:05 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DEFAULT_PATH "/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"

int	shield_path_check(const char *name)
{
	const char *path = getenv("PATH");
	if (!path || !*path)
		path = DEFAULT_PATH;
	char *our_path = (char *) calloc(strlen(path) + 2, sizeof(char));
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
			char *next = (char *) calloc(strlen(curr) + strlen(name) + 2, sizeof(char));
			if (next)
			{
				strcpy(next, curr);
				strcat(next, "/");
				strcat(next, name);
				if (access(next, F_OK | X_OK) == 0)
					found = 1;
			}
			free(next);
			if (found)
				break ;
			curr = curr + strlen(curr) + 1;
		}
		free(our_path);
	}
	return (found);
}
