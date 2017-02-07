/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#include <errno.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include <squish/distribution.h>

long
buffer_distribution(struct distribution *dist, void *buffer, size_t size)
{
	unsigned char *bytes = buffer;

	if (!dist || !buffer || !size)
		return -EINVAL;

	for (size_t i = 0; i < size; i += 1)
		dist->counts[bytes[i]] += 1;
	dist->total += size;

	return 0;
}

long
distribution_adjust(struct distribution *dist)
{
	int needs_adjustment = 0;

	if (!dist)
		return -EINVAL;

	for (size_t i = 0; i < ALPHABET_SIZE; i += 1) {
		if (dist->counts[i] == 0) {
			needs_adjustment = 1;
			break;
		}
	}

	if (needs_adjustment) {
		for (size_t i = 0; i < ALPHABET_SIZE; i += 1)
			dist->counts[i] += 1;
		dist->total += ALPHABET_SIZE;
	}

	return 0;
}

long
distribution_free(struct distribution *dist)
{
	if (!dist)
		return -EINVAL;

	free(dist);

	return 0;
}

struct distribution *
distribution_new()
{
	return calloc(1, sizeof(struct distribution));
}

long
file_distribution(struct distribution *dist, int fd)
{
	int status;
	off_t start, size;
	void *buffer;

	if (!dist || fd < 0)
		return -EINVAL;

	/* Find the current offset and the size of the file. */
	if ((start = lseek(fd, 0, SEEK_CUR)) == -1)
		return -errno;
	if ((size = lseek(fd, 0, SEEK_END)) == -1)
		return -errno;
	/* Ensure size does not become zero or negative. */
	if (size <= start)
		return -EBADF;
	/* The size of the buffer is the length of the file after the current point. */
	size -= start;
	if ((buffer = mmap(0, size, PROT_READ, MAP_PRIVATE, fd, start)) == MAP_FAILED)
		return -errno;
	status = buffer_distribution(dist, buffer, size);
	if ((munmap(buffer, size)) == -1)
		return -errno;

	return status;
}
