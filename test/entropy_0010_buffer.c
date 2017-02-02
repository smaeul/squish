/**
 * entropy_0010_buffer.c
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>

#include <squish/entropy.h>

#define SIZE 64

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
	unsigned char buffer[SIZE] = { 0 };
	struct entctx *c1;

	c1 = context_new();
	assert(c1);
	assert(iszero(c1, sizeof(struct entctx)));

	/* Try some noop failure cases. */
	assert(count_buffer(0, buffer, sizeof(buffer)) == -EINVAL);
	assert(iszero(buffer, sizeof(buffer)));
	assert(iszero(c1, sizeof(struct entctx)));
	assert(count_buffer(c1, 0, sizeof(buffer)) == -EINVAL);
	assert(iszero(buffer, sizeof(buffer)));
	assert(iszero(c1, sizeof(struct entctx)));
	assert(count_buffer(c1, buffer, 0) == -EINVAL);
	assert(iszero(buffer, sizeof(buffer)));
	assert(iszero(c1, sizeof(struct entctx)));

	assert(context_free(c1) == 0);

	return 0;
}
