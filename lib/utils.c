/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include <imgcomp/imgcomp.h>

int
readall(int fd, void *buffer, size_t size)
{
	size_t finished = 0;
	ssize_t current;
	uint8_t *bytebuf = buffer;

	if (!buffer || !size)
		return -EFAULT;

	while (finished < size) {
		if ((current = read(fd, bytebuf + finished, size - finished)) < 0) {
			/* Always retry if interrupted; never retry after other errors. */
			if (errno != EINTR)
				return -errno;
		} else if (current == 0) {
			return -EBADF;
		} else {
			/* Add this round to the total if it actually represents a number of bytes. */
			finished += current;
		}
	}

	return 0;
}

int
writeall(int fd, void *buffer, size_t size)
{
	size_t finished = 0;
	ssize_t current;
	uint8_t *bytebuf = buffer;

	if (!buffer || !size)
		return -EFAULT;

	while (finished < size) {
		if ((current = write(fd, bytebuf + finished, size - finished)) < 0) {
			/* Always retry if interrupted; never retry after other errors. */
			if (errno != EINTR)
				return -errno;
		} else if (current == 0) {
			return -EBADF;
		} else {
			/* Add this round to the total if it actually represents a number of bytes. */
			finished += current;
		}
	}

	return 0;
}
