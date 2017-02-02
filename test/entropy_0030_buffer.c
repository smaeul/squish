/**
 * entropy_0030_buffer.c
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
	struct entctx *c1;

	c1 = context_new();
	assert(c1);

	/* Failure case: null pointer. */
	assert(calc_entropy(0, &entropy) == -EINVAL);
	assert(entropy == SENTINEL);

	/* Failure case: null pointer again. */
	assert(calc_entropy(c1, 0) == -EINVAL);
	assert(entropy == SENTINEL);

	/* Failure case: zero length. */
	assert(calc_entropy(c1, &entropy) == -EDOM);
	assert(entropy == SENTINEL);

	/* Failure case: data does not match length. */
	c1->inputsize = 1;
	assert(calc_entropy(c1, &entropy) == -EDOM);
	assert(entropy == SENTINEL);

	/* Failure case: data does not match length again. */
	c1->counts[100] = 9000;
	c1->inputsize = 9999;
	assert(calc_entropy(c1, &entropy) == -EDOM);
	assert(entropy == SENTINEL);

	assert(context_free(c1) == 0);

	return 0;
}
