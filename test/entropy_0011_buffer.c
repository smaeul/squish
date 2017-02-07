/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

#include <squish/entropy.h>

#define SIZE 64

int
main(void)
{
	unsigned char buffer[SIZE] = { 0 };
	struct distribution *dist;

	dist = new_distribution();
	assert(dist);

	/* Success case #1. */
	assert(buffer_distribution(dist, buffer, sizeof(buffer)) == 0);
	assert(dist->total == SIZE);
	assert(dist->counts[0] == SIZE);
	for (size_t i = 1; i < ALPHABET_SIZE; i += 1)
		assert(dist->counts[i] == 0);

	assert(free_distribution(dist) == 0);

	return 0;
}
