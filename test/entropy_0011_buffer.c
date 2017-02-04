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
	struct entctx *c1;

	c1 = context_new();
	assert(c1);

	/* Success case #1. */
	assert(count_buffer(c1, buffer, sizeof(buffer)) == 0);
	assert(c1->inputsize == SIZE);
	assert(c1->counts[0] == SIZE);
	for (size_t i = 1; i < ALPHABET_SIZE; i += 1)
		assert(c1->counts[i] == 0);

	assert(context_free(c1) == 0);

	return 0;
}
