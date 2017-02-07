/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#include <errno.h>
#include <math.h>
#include <stddef.h>

#include <squish/entropy.h>

long
calculate_entropy(struct distribution *dist, double *result)
{
	double esum = 0;
	size_t csum = 0;

	if (!dist || !result)
		return -EINVAL;

	/* Avoid division by zero. */
	if (!dist->total)
		return -EDOM;
	for (size_t i = 0; i < ALPHABET_SIZE; i += 1) {
		double p = (double) dist->counts[i] / (double) dist->total;

		/* log2(0) is undefined, but we would multiply it by zero anyway, so ignore that case. */
		if (p == 0)
			continue;
		/* Since 0 < p ≤ 1, log2(p) is negative, so by subtracting we get a positive sum. */
		esum -= p * log2(p);
		/* Sum up the character counts as a check on the input. */
		csum += dist->counts[i];
	}
	if (csum != dist->total)
		return -EDOM;
	*result = esum;

	return 0;
}
