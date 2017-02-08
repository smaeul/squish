/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#include <assert.h>
#include <string.h>

#include <internal/utils.h>

int
main(void)
{
	char *r1 = "hello", *r2 = "world", *s1, *s2;

	assert((s1 = strdup(r1)));
	assert((s2 = strdup(r2)));

	memswap(s1, s2, sizeof(s1));

	assert(strcmp(s1, r2) == 0);
	assert(strcmp(s2, r1) == 0);

	return 0;
}
