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
#define SIZE 64

int
main(void)
{
	double entropy = SENTINEL;
	unsigned char buffer[SIZE] = { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15,
		                           16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
		                           32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
		                           48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63 };
	struct entctx *c1;

	c1 = context_new();
	assert(c1);

	/* Success case #4. */
	assert(count_buffer(c1, buffer, sizeof(buffer)) == 0);
	assert(c1->inputsize == SIZE);
	assert(calc_entropy(c1, &entropy) == 0);
	assert(entropy == 6);

	assert(context_free(c1) == 0);

	return 0;
}
