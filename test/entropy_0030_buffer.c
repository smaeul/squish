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

#define SENTINEL 42.42

int
main(void)
{
	double entropy = SENTINEL;
	struct distribution *dist;

	dist = new_distribution();
	assert(dist);

	/* Failure case: null pointer. */
	assert(calculate_entropy(0, &entropy) == -EINVAL);
	assert(entropy == SENTINEL);

	/* Failure case: null pointer again. */
	assert(calculate_entropy(dist, 0) == -EINVAL);
	assert(entropy == SENTINEL);

	/* Failure case: zero length. */
	assert(calculate_entropy(dist, &entropy) == -EDOM);
	assert(entropy == SENTINEL);

	/* Failure case: data does not match length. */
	dist->total = 1;
	assert(calculate_entropy(dist, &entropy) == -EDOM);
	assert(entropy == SENTINEL);

	/* Failure case: data does not match length again. */
	dist->counts[100] = 9000;
	dist->total = 9999;
	assert(calculate_entropy(dist, &entropy) == -EDOM);
	assert(entropy == SENTINEL);

	assert(free_distribution(dist) == 0);

	return 0;
}
