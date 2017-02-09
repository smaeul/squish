/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#include <errno.h>
#include <limits.h>
#include <unistd.h>

#include <internal/io.h>

ssize_t
write_retry(int fd, void *buffer, size_t size, ssize_t minimum)
{
	ssize_t current, written = 0;

	if (fd < 0)
		return -EBADF;
	if (!buffer || !size || size > SSIZE_MAX)
		return -EINVAL;

	while (written < (ssize_t) size) {
		if ((current = write(fd, (unsigned char *) buffer + written, size - written)) == -1) {
			/* Always retry if interrupted. Never retry after other errors. */
			if (errno == EINTR)
				continue;
			else
				break;
		}
		/* Add this round to the total if it actually represents a number of bytes. */
		written += current;
		/* Only retry if this call wrote enough bytes. */
		if (current < minimum)
			break;
	}

	return written ? written : -errno;
}
