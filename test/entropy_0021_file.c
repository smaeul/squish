/**
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
	struct distribution *dist;

	assert(argc >= 2);
	assert(argv[1]);
	dir = open(argv[1], O_DIRECTORY);

	dist = new_distribution();
	assert(dist);

	/* Success case #1. */
	if (dir >= 0 && (fd = openat(dir, "zeroes.txt", O_RDONLY)) >= 0) {
		assert(file_distribution(dist, fd) == 0);
		close(fd);
	}
	assert(dist->total == SIZE);
	assert(dist->counts[0] == SIZE);
	for (size_t i = 1; i < ALPHABET_SIZE; i += 1)
		assert(dist->counts[i] == 0);

	assert(free_distribution(dist) == 0);

	close(dir);
	return 0;
}
