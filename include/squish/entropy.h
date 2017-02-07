/**
 * Copyright © 2017 Samuel Holland <samuel@sholland.org>
 * See LICENSE in the project directory for license terms.
 * vim: ft=c:noexpandtab:sts=4:sw=4:ts=4:tw=100
 */

#ifndef SQUISH_ENTROPY_H
#define SQUISH_ENTROPY_H

#include <squish/distribution.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Calculate the entropy of the data represented by the distribution structure.
 * @param dist The distribution structure to read from
 * @param result A double-precision variable where the entropy will be stored on success
 * @return 0 if the operation was successful, or a negative value representing an error
 */
long calculate_entropy(struct distribution *dist, double *result);

#ifdef __cplusplus
}
#endif

#endif /* SQUISH_ENTROPY_H */
