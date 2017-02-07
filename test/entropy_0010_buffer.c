/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>

#include <squish/entropy.h>

#define SIZE 64

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
main(void)
{
	unsigned char buffer[SIZE] = { 0 };
	struct distribution *dist;

	dist = distribution_new();
	assert(dist);
	assert(iszero(dist, sizeof(struct distribution)));

	/* Try some noop failure cases. */
	assert(buffer_distribution(0, buffer, sizeof(buffer)) == -EINVAL);
	assert(iszero(buffer, sizeof(buffer)));
	assert(iszero(dist, sizeof(struct distribution)));
	assert(buffer_distribution(dist, 0, sizeof(buffer)) == -EINVAL);
	assert(iszero(buffer, sizeof(buffer)));
	assert(iszero(dist, sizeof(struct distribution)));
	assert(buffer_distribution(dist, buffer, 0) == -EINVAL);
	assert(iszero(buffer, sizeof(buffer)));
	assert(iszero(dist, sizeof(struct distribution)));

	assert(distribution_free(dist) == 0);

	return 0;
}
