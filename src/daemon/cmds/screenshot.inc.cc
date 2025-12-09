/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   screenshot.inc.cc                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 22:02:48 by kiroussa          #+#    #+#             */
/*   Updated: 2025/12/09 23:08:27 by kiroussa         ###   ########.fr       */
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
#include <shield.h>
#include <immintrin.h>
#include <unistd.h>

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

static inline int	shield_tempfile0(const char *prefix, const char *suffix, size_t depth)
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
			fd = open(buffer, O_CREAT | O_EXCL | O_RDWR, 0644);
		// make sure to exit the scope to cleanup the stack from the buffer
	}
	if (fd == -2)
		return shield_tempfile0(prefix, suffix, depth + 1);
	return fd;
}

static inline int	shield_tempfile(const char *prefix, const char *suffix)
{
	return shield_tempfile0(prefix, suffix, 0);
}

static inline void close_file(int *fd)
{
	if (fd && *fd >= 0)
		close(*fd);
}

// Thank you stackoverflow <3
// https://stackoverflow.com/q/8249669
[[gnu::used]]
static const char *shield_screenshot(void)
{
	if (getenv("WAYLAND_DISPLAY"))
		return (ERR "Wayland is not supported due to budget restrictions");

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

	[[gnu::cleanup(close_file)]]
	int fd = shield_tempfile("screenshot", ".ppm");
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

	return ("Success");
}

#ifdef SCREENSHOT_MAIN

int main(int argc, char **argv)
{
	printf("%s\n", shield_screenshot());
	return (0);
}

#endif
