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
	unsigned char buffer[SIZE] = { 0 };
	struct distribution *dist;

	dist = distribution_new();
	assert(dist);

	/* Success case #2. */
	assert(buffer_distribution(dist, buffer, sizeof(buffer)) == 0);
	assert(distribution_adjust(dist) == 0);
	assert(dist->total == SIZE + ALPHABET_SIZE);
	assert(dist->counts[0] == SIZE + 1);
	for (size_t i = 1; i < ALPHABET_SIZE; i += 1)
		assert(dist->counts[i] == 1);

	/* Idempotence test. */
	assert(distribution_adjust(dist) == 0);
	assert(dist->total == SIZE + ALPHABET_SIZE);
	assert(dist->counts[0] == SIZE + 1);
	for (size_t i = 1; i < ALPHABET_SIZE; i += 1)
		assert(dist->counts[i] == 1);

	assert(distribution_free(dist) == 0);

	return 0;
}
