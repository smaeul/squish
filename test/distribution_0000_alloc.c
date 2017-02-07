/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

#include <squish/distribution.h>

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
	struct distribution *d1, *d2, *d3;

	d1 = distribution_new();
	assert(d1);
	assert(iszero(d1, sizeof(struct distribution)));

	d2 = distribution_new();
	assert(d2);
	assert(d2 != d1);
	assert(iszero(d2, sizeof(struct distribution)));

	d3 = distribution_new();
	assert(d3);
	assert(d3 != d1);
	assert(d3 != d2);
	assert(iszero(d3, sizeof(struct distribution)));

	assert(distribution_free(d1) == 0);
	assert(distribution_free(d2) == 0);
	assert(distribution_free(d3) == 0);

	return 0;
}
