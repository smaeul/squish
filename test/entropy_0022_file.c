/**
 * entropy_0022_file.c
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

#define SIZE 4096

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

	/* Success case #1. */
	if (dir >= 0 && (fd = openat(dir, "ones.txt", O_RDONLY)) >= 0) {
		assert(count_file(c1, fd) == 0);
		close(fd);
	}
	assert(c1->inputsize == SIZE);
	for (size_t i = 0; i < 255; i += 1)
		assert(c1->counts[i] == 0);
	assert(c1->counts[255] == SIZE);

	assert(context_free(c1) == 0);

	close(dir);
	return 0;
}
