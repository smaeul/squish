/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>

#include <squish/distribution.h>

#define SIZE 64

int
main(void)
{
	/* Failure case. */
	assert(distribution_adjust(0) == -EINVAL);

	return 0;
}
