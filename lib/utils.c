/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

#include <imgcomp/imgcomp.h>

int
readall(int fd, uint8_t *buffer, size_t size)
{
	size_t finished = 0;
	ssize_t current;

	if (!buffer || !size)
		return ERR_NULL;

	while (finished < size) {
		if ((current = read(fd, buffer + finished, size - finished)) < 0) {
			/* Always retry if interrupted; never retry after other errors. */
			if (errno != EINTR)
				return -errno;
		} else if (current == 0) {
			return ERR_NODATA;
		} else {
			/* Add this round to the total if it actually represents a number of bytes. */
			finished += current;
		}
	}

	return 0;
}

int
writeall(int fd, uint8_t *buffer, size_t size)
{
	size_t finished = 0;
	ssize_t current;

	if (!buffer || !size)
		return ERR_NULL;

	while (finished < size) {
		if ((current = write(fd, buffer + finished, size - finished)) < 0) {
			/* Always retry if interrupted; never retry after other errors. */
			if (errno != EINTR)
				return -errno;
		} else if (current == 0) {
			return ERR_NODATA;
		} else {
			/* Add this round to the total if it actually represents a number of bytes. */
			finished += current;
		}
	}

	return 0;
}
