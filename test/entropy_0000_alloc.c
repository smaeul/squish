/**
 * entropy_0000_alloc.c
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

#include <squish/entropy.h>

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
	struct entctx *c1, *c2, *c3;

	c1 = context_new();
	assert(c1);
	assert(iszero(c1, sizeof(struct entctx)));

	c2 = context_new();
	assert(c2);
	assert(c2 != c1);
	assert(iszero(c2, sizeof(struct entctx)));

	c3 = context_new();
	assert(c3);
	assert(c3 != c1);
	assert(c3 != c2);
	assert(iszero(c3, sizeof(struct entctx)));

	assert(context_free(c1) == 0);
	assert(context_free(c2) == 0);
	assert(context_free(c3) == 0);

	return 0;
}
