/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

#include <squish/distribution.h>

#define SIZE 64

int
main(void)
{
	unsigned char buffer[SIZE] = { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15,
		                           16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
		                           32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
		                           48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63 };
	struct distribution *dist;

	dist = distribution_new();
	assert(dist);

	/* Success case #2. */
	assert(buffer_distribution(dist, buffer, sizeof(buffer)) == 0);
	assert(dist->total == SIZE);
	for (size_t i = 0; i < SIZE; i += 1)
		assert(dist->counts[i] == 1);
	for (size_t i = SIZE; i < ALPHABET_SIZE; i += 1)
		assert(dist->counts[i] == 0);

	assert(distribution_free(dist) == 0);

	return 0;
}
