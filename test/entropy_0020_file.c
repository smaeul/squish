/**
 * entropy_0020_file.c
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <unistd.h>

#include <squish/entropy.h>

static bool
iszero(void *buffer, size_t size)
{
	unsigned char *bytes = buffer;

	if (!buffer || !size)
		return false;

	for (size_t i = 0; i < size; i += 1)
		if (bytes[i])
			return false;

	return true;
}

int
main(int argc, char *argv[])
{
	int dir, fd;
	struct entctx *c1;

	assert(argc >= 2);
	assert(argv[1]);
	dir = open(argv[1], O_DIRECTORY);

	c1 = context_new();
	assert(c1);

	/* Failure case: bad fd. */
	assert(count_file(c1, -1) == -EINVAL);
	assert(iszero(c1, sizeof(struct entctx)));

	/* Failure case: empty device. */
	if ((fd = open("/dev/null", O_RDONLY)) >= 0) {
		assert(count_file(c1, fd) == -EBADF);
		close(fd);
	}
	assert(iszero(c1, sizeof(struct entctx)));

	/* Failure case: device open for writing. */
	if ((fd = open("/dev/null", O_WRONLY)) >= 0) {
		assert(count_file(c1, fd) == -EBADF);
		close(fd);
	}
	assert(iszero(c1, sizeof(struct entctx)));

	/* Failure case: empty file. */
	if (dir >= 0 && (fd = openat(dir, "empty.txt", O_RDONLY)) >= 0) {
		assert(count_file(c1, fd) == -EBADF);
		close(fd);
	}
	assert(iszero(c1, sizeof(struct entctx)));

	/* Failure case: file open for writing. */
	if (dir >= 0 && (fd = openat(dir, "empty.txt", O_WRONLY)) >= 0) {
		assert(count_file(c1, fd) == -EBADF);
		close(fd);
	}
	assert(iszero(c1, sizeof(struct entctx)));

	assert(context_free(c1) == 0);

	close(dir);
	return 0;
}
