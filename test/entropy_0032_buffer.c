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

	/* Success case #2. */
	dist->counts[100] = 2;
	dist->total = 2;
	assert(calculate_entropy(dist, &entropy) == 0);
	assert(entropy == 0);

	assert(free_distribution(dist) == 0);

	return 0;
}
