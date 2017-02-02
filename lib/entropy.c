/**
 * entropy.c - routines for calculating entropy
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#include <errno.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include <squish/entropy.h>

int
calc_entropy(struct entctx *context, double *result)
{
	double esum = 0;
	size_t csum = 0;

	if (!context || !result)
		return -EINVAL;

	/* Avoid division by zero. */
	if (!context->inputsize)
		return -EDOM;
	for (size_t i = 0; i < ALPHABET_SIZE; i += 1) {
		double p = (double) context->counts[i] / (double) context->inputsize;

		/* log2(0) is undefined, but we would multiply it by zero anyway, so ignore that case. */
		if (p == 0)
			continue;
		/* Since 0 < p ≤ 1, log2(p) is negative, so by subtracting we get a positive sum. */
		esum -= p * log2(p);
		/* Sum up the character counts as a check on the input. */
		csum += context->counts[i];
	}
	if (csum != context->inputsize)
		return -EDOM;
	*result = esum;

	return 0;
}

struct entctx *
context_new()
{
	return calloc(1, sizeof(struct entctx));
}

int
context_free(struct entctx *context)
{
	if (!context)
		return -EINVAL;

	free(context);

	return 0;
}

int
count_buffer(struct entctx *context, void *buffer, size_t size)
{
	unsigned char *bytes = buffer;

	if (!context || !buffer || !size)
		return -EINVAL;

	for (size_t i = 0; i < size; i += 1)
		context->counts[bytes[i]] += 1;
	context->inputsize += size;

	return 0;
}

int
count_file(struct entctx *context, int fd)
{
	int status;
	off_t start, size;
	void *buffer;

	if (!context || fd < 0)
		return -EINVAL;

	/* Find the current offset and the size of the file. */
	if ((start = lseek(fd, 0, SEEK_CUR)) == -1)
		return -errno;
	if ((size = lseek(fd, 0, SEEK_END)) == -1)
		return -errno;
	/* Ensure size does not become zero or negative. */
	if (size <= start)
		return -EBADF;
	/* The size of the buffer is the size of the file after the current point. */
	size -= start;
	if ((buffer = mmap(0, size, PROT_READ, MAP_PRIVATE, fd, start)) == MAP_FAILED)
		return -errno;
	status = count_buffer(context, buffer, size);
	if ((munmap(buffer, size)) == -1)
		return -errno;

	return status;
}
