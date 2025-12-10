/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   screenshot.inc.cc                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 22:02:48 by kiroussa          #+#    #+#             */
/*   Updated: 2025/12/10 01:25:12 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <limits.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <grp.h>
#include <pwd.h>
#include <shield.h>
#include <immintrin.h>
#include <unistd.h>

[[gnu::weak]]
int	shield_path_check(const char *name);

#define ERR "ERROR|"

static inline void shield_random_string(size_t length, char *buffer)
{
	static const char charset[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

	uint16_t tmp = (uint16_t) 0xDEADBEEF;
	_rdrand16_step(&tmp);
	uint64_t value = tmp;
	value *= 0x9e3779b97f4a7c15LL;
	value ^= (value >> 30);
	value *= 0xbf58476d1ce4e5b9LL;
	value ^= (value >> 27);

	for (size_t i = 0; i < length; i++)
	{
		buffer[i] = charset[(value) % (sizeof(charset) - 1)];
		value *= 0x9e3779b97f4a7c15LL;
		value ^= (value >> 30);
		value *= 0xbf58476d1ce4e5b9LL;
		value ^= (value >> 27);
	}
	buffer[length] = '\0';
}

static inline int	shield_tempfile0(const char *prefix, const char *suffix, char *file_name, size_t depth)
{
	int fd = -1;
	if (depth < 64)
	{
		char *tmpdir = getenv("TMPDIR");
		if (!tmpdir)
			tmpdir = (char *) "/tmp";
		char buffer[PATH_MAX + 1];
		memset(buffer, 0, sizeof(buffer));
		strlcat(buffer, tmpdir, PATH_MAX);
		strlcat(buffer, "/", PATH_MAX);
		strlcat(buffer, prefix, PATH_MAX);
		size_t len = strlen(buffer);
#define NCHARS 32
		if (len >= PATH_MAX - NCHARS)
			return (-1);
		shield_random_string(NCHARS, buffer + len);
#undef NCHARS
		strlcat(buffer, suffix, PATH_MAX);

		if (access(buffer, F_OK) == 0)
			fd = -2;
		else
		{
			fd = open(buffer, O_CREAT | O_EXCL | O_RDWR, 0644);
			if (fd > 0)
				strlcpy(file_name, buffer, PATH_MAX);
		}
		// make sure to exit the scope to cleanup the stack from the buffer
	}
	if (fd == -2)
		return shield_tempfile0(prefix, suffix, file_name, depth + 1);
	return fd;
}

static inline int	shield_tempfile(const char *prefix, const char *suffix, char *file_name)
{
	return shield_tempfile0(prefix, suffix, file_name, 0);
}

static inline void	shield_drop_privileges(void)
{
	if (getuid() != 0)
		return;
	uid_t uid = 1000;
	struct passwd *pw = getpwuid(uid);
	if (!pw)
		return;
	gid_t gid = pw->pw_gid;
	initgroups(pw->pw_name, gid);
	setresgid(gid, gid, gid);
	setresuid(uid, uid, uid);

	setenv("USER", pw->pw_name, 1);
    setenv("LOGNAME", pw->pw_name, 1);
    setenv("HOME", pw->pw_dir, 1);
    setenv("SHELL", pw->pw_shell, 1);

	setenv("DISPLAY", ":0", 1);
}

static const char *shield_take_screenshot(void)
{
	Display *display = XOpenDisplay(NULL);
	if (!display)
	{
		DEBUG("Failed to open display: %m\n");
		return (ERR "Failed to open display");
	}
	Window root = DefaultRootWindow(display);

	XWindowAttributes gwa;
	if (XGetWindowAttributes(display, root, &gwa) == 0)
	{
		DEBUG("Failed to get window attributes: %m\n");
		return (ERR "Failed to get window attributes");
	}
	int width = gwa.width;
	int height = gwa.height;

	DEBUG("width: %d\n", width);
	DEBUG("height: %d\n", height);

	XImage *image = XGetImage(display, root, 0, 0, width, height, AllPlanes, XYPixmap);
	if (!image)
	{
		DEBUG("Failed to get image: %m\n");
		return (ERR "Failed to get image");
	}
	DEBUG("image: %p\n", image);

	char file_name[PATH_MAX + 1];
	memset(file_name, 0, sizeof(file_name));

	int fd = shield_tempfile("screenshot", ".ppm", file_name);
	if (fd == -1)
	{
		DEBUG("Failed to create tempfile: %m\n");
		return (ERR "Failed to create tempfile");
	}

	unsigned long red_mask = image->red_mask;
	unsigned long green_mask = image->green_mask;
	unsigned long blue_mask = image->blue_mask;

	write(fd, "P6\n", 3);
	dprintf(fd, "%d %d\n255\n", width, height);
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			unsigned long pixel = XGetPixel(image, x, y);

			unsigned char blue = pixel & blue_mask;
			unsigned char green = (pixel & green_mask) >> 8;
			unsigned char red = (pixel & red_mask) >> 16;
			write(fd, &red, 1);
			write(fd, &green, 1);
			write(fd, &blue, 1);
		}
	}
	close(fd);

#ifdef SCREENSHOT_MAIN
	if (true)
#else
	if (shield_path_check != NULL && shield_path_check("ffmpeg"))
#endif
	{
		char buffer[2048];
		memset(buffer, 0, sizeof(buffer));
		strlcat(buffer, "ffmpeg -y -i ", sizeof(buffer));
		strlcat(buffer, file_name, sizeof(buffer));
		strlcat(buffer, " -compression_level 9 ", sizeof(buffer));
		char changed_out[sizeof(file_name)];
		memcpy(changed_out, file_name, sizeof(file_name));
		char *where = strstr(changed_out, ".ppm");
		if (where)
			memcpy(where, ".png", sizeof(".png"));
		strlcat(buffer, changed_out, sizeof(buffer));
		int ret = system(buffer);
		if (ret == 0)
		{
			unlink(file_name);
			memcpy(file_name, changed_out, sizeof(file_name));
		}
	}
	
	return ("Success");
}

// Thank you stackoverflow <3
// https://stackoverflow.com/q/8249669
[[gnu::used]]
static const char *shield_screenshot(void)
{
	if (getenv("WAYLAND_DISPLAY"))
		return (ERR "Wayland is not supported due to budget restrictions");

	int fds[2];
	if (pipe(fds) == -1)
		return (ERR "Failed to create pipe");

	int pid = fork();
	if (pid == -1)
	{
		close(fds[0]);
		close(fds[1]);
		return (ERR "Failed to fork");
	}

	if (pid == 0)
	{
		close(fds[0]);
		shield_drop_privileges();
		// setenv("DISPLAY", ":0", 0);
		const char *result = shield_take_screenshot();
		write(fds[1], result, strlen(result));
		close(fds[1]);
		_exit(0);
	}
	close(fds[1]);
	waitpid(pid, NULL, 0);

	static char result[PATH_MAX + 1];
	memset(result, 0, sizeof(result));
	read(fds[0], result, PATH_MAX);
	close(fds[0]);

	return (result);
}

#ifdef SCREENSHOT_MAIN

int main(int argc, char **argv)
{
	printf("%s\n", shield_screenshot());
	return (0);
}

#endif
