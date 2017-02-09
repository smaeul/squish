/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#include <limits.h>

#include <internal/utils.h>

uint32_t
log2u32(uint32_t n)
{
	/* __builtin_clz() is not defined if the input is zero. This is not mathematically accurate, but
	 * is at least well-defined. */
	if (!n)
		return 0;

	return CHAR_BIT * sizeof(n) - __builtin_clz(n) - 1;
}

void
memswap(void *a, void *b, size_t n)
{
	unsigned char *x = a, *y = b, z;

	if (!a || !b || !n)
		return;

	for (size_t i = 0; i < n; i += 1) {
		z = x[i];
		x[i] = y[i];
		y[i] = z;
	}
}
