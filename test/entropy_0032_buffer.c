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
	struct entctx *c1;

	c1 = context_new();
	assert(c1);

	/* Success case #2. */
	c1->counts[100] = 2;
	c1->inputsize = 2;
	assert(calc_entropy(c1, &entropy) == 0);
	assert(entropy == 0);

	assert(context_free(c1) == 0);

	return 0;
}
